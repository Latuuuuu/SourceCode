#include <allegro5/allegro_audio.h>
#include "gamescene.h"
#include "../element/element.h"
#include "../element/charater.h"
#include "../element/floor.h"
#include "../element/teleport.h"
#include "../element/tree.h"
#include "../element/projectile.h"
#include "../element/Ball.h"
#include "../element/susu.h"
#include "../element/tungtungtung.h"
#include "../element/trippi_troppi.h"
#include "../element/capuccino.h"
#include "../element/bananini.h"
#include "../element/patapim.h"
#include "../element/hpbar.h"
#include "../element/tralala.h"
#include "../element/crocodilo.h"
#include "../element/bigtung.h"
#include "../element/level_switch.h"
#include "../global.h"
#include <stdio.h>
#include "../element/monster_factory.h"
#include <allegro5/allegro_primitives.h>


static bool is_paused = false;
static int pause_option = 0; // 0 = Resume, 1 = Reset, 2 = Main Menu
ALLEGRO_FONT *pause_font = NULL;

void Load_Map_And_Generate_Tile(Scene *scene) {
    FILE *fp = fopen("assets/map/map.txt", "r");
    if (!fp) {
        fprintf(stderr, "Failed to open map.txt\n");
        exit(1);
    }

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH;) {
            char ch = fgetc(fp);
            if (ch == '0' || ch == '1') {
                map[y][x] = ch - '0';
                x++;
            }
        }
    }
    fclose(fp);

    floor_tile = al_load_bitmap("assets/image/floor_tile.png");
    wall_tile = al_load_bitmap("assets/image/wall_tile.png");

    if (!floor_tile || !wall_tile) {
        fprintf(stderr, "Failed to load tile images\n");
        exit(1);
    }
}

/*
   [GameScene function]
*/
static double _prev_time = 0.0;

Scene *New_GameScene(int label)
{
    GameScene *pDerivedObj = (GameScene *)malloc(sizeof(GameScene));
    Scene *pObj = New_Scene(label);
    // setting derived object member
    pDerivedObj->background = al_load_bitmap("assets/image/stage.jpg");
    pObj->pDerivedObj = pDerivedObj;
    Load_Map_And_Generate_Tile(pObj);
    // register static elements (background decorations / player)
    //_Register_elements(pObj, New_Floor(Floor_L));
    //_Register_elements(pObj, New_Teleport(Teleport_L));
    //_Register_elements(pObj, New_Tree(Tree_L));
    //_Register_elements(pObj, New_Character(Character_L));
    _Register_elements(pObj, New_susu(Susu_L));
    _Register_elements(pObj, New_Hpbar(Hpbar_L,10000,10000));
    //_Register_elements(pObj, New_tungtungtung(tungtungtung_L));
    //_Register_elements(pObj, New_trippi_troppi(trippi_troppi_L));

    // initialise monster factory (optional reset)
    MF_Reset();
    Level_switch_Init();
    pause_font = al_load_ttf_font("assets/font/pirulen.ttf", 48, 0);
    // setting derived object function
    pObj->Update = game_scene_update;
    pObj->Draw = game_scene_draw;
    pObj->Destroy = game_scene_destroy;
    return pObj;
}

void game_scene_update(Scene *self)
{
    if (key_state[ALLEGRO_KEY_ESCAPE]) {
        is_paused = !is_paused;
        key_state[ALLEGRO_KEY_ESCAPE] = false;
    }

    if (is_paused) {
        if (key_state[ALLEGRO_KEY_UP])
        {
            pause_option = (pause_option + 2) % 3;
            key_state[ALLEGRO_KEY_UP] = false;
        }
        if (key_state[ALLEGRO_KEY_DOWN])
        {
            pause_option = (pause_option + 1) % 3;
            key_state[ALLEGRO_KEY_DOWN] = false;                
        }
        if (key_state[ALLEGRO_KEY_ENTER])
        {
            key_state[ALLEGRO_KEY_ENTER] = false;
            if (pause_option == 0) is_paused = false;               // Resume
            else if (pause_option == 1)
            {                           // Reset
                self->scene_end = true;
                window = 1;
                is_paused= false;                     // Reset the game
            }
            else if (pause_option == 2)
            { 
                is_paused=false;                         // Main Menu
                self->scene_end = true;
                window = 0;
            }
        }
        return;
    }
    double now = al_get_time();
    if (_prev_time == 0.0) _prev_time = now;
    double dt = now - _prev_time;
    _prev_time = now;

    // let factory decide whether to spawn monsters this frame
    //MF_Update(self, dt);
    Level_switch_Update(self, dt);


    // update every element
    ElementVec allEle = _Get_all_elements(self);
    for (int i = 0; i < allEle.len; i++) {
        Elements *ele = allEle.arr[i];
        ele->Update(ele);
    }
    for (int i = 0; i < allEle.len; i++) {
        Elements *ele = allEle.arr[i];
        ele->Interact(ele);
    }
    // remove element that marked delete
    for (int i = 0; i < allEle.len; i++)
    {
        Elements *ele = allEle.arr[i];
        if (ele->dele)
            _Remove_elements(self, ele);
    }
}

void game_scene_draw(Scene *self)
{
    al_clear_to_color(al_map_rgb(0, 0, 0));
    
    // draw map tiles
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            ALLEGRO_BITMAP *tile = (map[y][x] == 1) ? wall_tile : floor_tile;
            al_draw_bitmap(tile, x * TILE_SIZE, y * TILE_SIZE, 0);
        }
    }

    ElementVec allEle = _Get_all_elements(self);
    for (int i = 0; i < allEle.len; i++) {
        Elements *ele = allEle.arr[i];
        ele->Draw(ele);
    }
    Level_switch_DrawOverlay();
    if (is_paused)
    {
        al_draw_filled_rectangle(0, 0, WIDTH, HEIGHT, al_map_rgba(0, 0, 0, 160));
        al_draw_text(pause_font, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 - 80, ALLEGRO_ALIGN_CENTRE, "Paused");
        al_draw_text(pause_font, pause_option == 0 ? al_map_rgb(255, 255, 0) : al_map_rgb(200, 200, 200), WIDTH / 2, HEIGHT / 2 + 0, ALLEGRO_ALIGN_CENTRE, "Continue");
        al_draw_text(pause_font, pause_option == 1 ? al_map_rgb(255, 255, 0) : al_map_rgb(200, 200, 200), WIDTH / 2, HEIGHT / 2 + 60, ALLEGRO_ALIGN_CENTRE, "Reset");
        al_draw_text(pause_font, pause_option == 2 ? al_map_rgb(255, 255, 0) : al_map_rgb(200, 200, 200), WIDTH / 2, HEIGHT / 2 + 120, ALLEGRO_ALIGN_CENTRE, "Main Menu");
    }
}

void game_scene_destroy(Scene *self)
{
    MF_Destroy(); // clean up factory‑related resources
    Level_switch_Destroy();

    GameScene *Obj = ((GameScene *)(self->pDerivedObj));
    ALLEGRO_BITMAP *background = Obj->background;
    al_destroy_bitmap(background);

    if (floor_tile) al_destroy_bitmap(floor_tile);
    if (wall_tile) al_destroy_bitmap(wall_tile);

    ElementVec allEle = _Get_all_elements(self);
    for (int i = 0; i < allEle.len; i++) {
        Elements *ele = allEle.arr[i];
        ele->Destroy(ele);
    }
    free(Obj);
    free(self);
}
