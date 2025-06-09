#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "level_switch.h"
#include "monster_factory.h"
#include "allegro5/allegro_primitives.h"
#include "../global.h"

static LevelState state = LVL_RUNNING;
static double     trans_timer = 0.0;
static int        level_no = 0;               
static ALLEGRO_FONT *level_font = NULL;
static ALLEGRO_FONT *story_font = NULL;
static ALLEGRO_BITMAP *intro_img = NULL; 
static double time_space = 10.0;

void Level_switch_Init(void)
{
    state = LVL_RUNNING;
    trans_timer = 0.0;
    level_no = 0; 
    time_space = 10.0;
    if (!level_font) level_font = al_load_ttf_font("assets/font/pirulen.ttf", 64, 0);
    if (!story_font) story_font = al_load_ttf_font("assets/font/pirulen.ttf", 36, 0);
    if (!intro_img)intro_img = al_load_bitmap("assets/image/intro.png");          
}

LevelState Level_switch_GetState(void) { return state; }

void Level_switch_Update(Scene *scene, double dt)
{
    switch (state) {
    case LVL_RUNNING:
        /* 1) 先讓 MonsterFactory 正常運作 */
        MF_Update(scene, dt);

        /* 2) 場上清光且下一波是 buffer → 進 TRANSITION */
        if (MF_AliveMonsterCount(scene) == 0 && MF_NextWaveIsBuffer()) {
            state = LVL_TRANSITION;
            trans_timer = 0.0;
        }
        break;

    case LVL_TRANSITION:
        trans_timer += dt;
        
        if(level_no==0)time_space=12.0;
        else time_space=10.0;
        if (trans_timer > time_space) {      /* 黑幕 5 秒 */
            MF_SkipBufferWave();       /* 跳過 buffer wave */
            ++level_no;
            state = LVL_RUNNING;
        }
        break;

    case LVL_FINISHED:
        /* 可擴充結束畫面 */
        break;
    }
}

void Level_switch_DrawOverlay(void)
{
    if (state != LVL_TRANSITION) return;

    /* 半透明黑幕 */
    al_draw_filled_rectangle(0,0,WIDTH,HEIGHT, al_map_rgb(0,0,0));
    if(time_space - trans_timer > 2)
    {
        if (story_font) 
        {
            char story[1000];
            switch (level_no) 
            {
                case 0:
                    if(trans_timer < 5.0)
                    {
                        sprintf(story, "One day, Susu wake up in Li Dormotory,\nfinging out something is strange in NTHU.\nThere are ai monsters eerywhere, \nbut Susu want to go to the library\nto finish is programming final project.\nWithout any hesitation,\nhe pick up his sword to clear all the monsters.");
                        //al_draw_text(level_font, al_map_rgb(255,255,255),WIDTH/2, 100, ALLEGRO_ALIGN_CENTRE, buf);
                        al_draw_multiline_text(story_font,al_map_rgb(255,255,255),WIDTH/2, HEIGHT/2-200,2000,40,ALLEGRO_ALIGN_CENTRE,story);
                    }
                    else
                    {
                        al_draw_bitmap(intro_img, 0, 100, 0);
                    }
                       
            }
        }
    }
    else
    {
        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT, al_map_rgb(0,0,0));
        /* 顯示文字 */
        if (level_font) {
            char buf[32];
            sprintf(buf, "LEVEL %d", level_no);
            al_draw_text(level_font, al_map_rgb(255,255,255),WIDTH/2, HEIGHT/2-32, ALLEGRO_ALIGN_CENTRE, buf);
        }
    }
}

void Level_switch_Destroy(void)
{
    if (level_font) {
        al_destroy_font(level_font);
        level_font = NULL;
    }
    if (story_font) {
        al_destroy_font(story_font);
        story_font = NULL;
    }
    
    if (intro_img) {
         al_destroy_bitmap(intro_img);
         intro_img = NULL;
    }
}