#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "level_switch.h"
#include "monster_factory.h"
#include "allegro5/allegro_primitives.h"
#include "../global.h"

static LevelState state = LVL_RUNNING;
static double     trans_timer = 0.0;
static int        level_no = 1;               /* 從 Level 1 開始 */
static ALLEGRO_FONT *level_font = NULL;

void Level_switch_Init(void)
{
    state = LVL_RUNNING;
    trans_timer = 0.0;
    level_no = 1;
    if (!level_font)
        level_font = al_load_ttf_font("assets/font/pirulen.ttf", 64, 0);
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
        if (trans_timer > 2.0) {      /* 黑幕 2 秒 */
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
    al_draw_filled_rectangle(0,0,WIDTH,HEIGHT, al_map_rgba(0,0,0,200));

    /* 顯示文字 */
    if (level_font) {
        char buf[32];
        sprintf(buf, "LEVEL %d", level_no+1);
        al_draw_text(level_font, al_map_rgb(255,255,255),
                     WIDTH/2, HEIGHT/2-32, ALLEGRO_ALIGN_CENTRE, buf);
    }
}