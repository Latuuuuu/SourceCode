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
#include "../global.h"
#include <stdio.h>
#include "../element/monster_factory.h"
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
    _Register_elements(pObj, New_tungtungtung(tungtungtung_L));
    _Register_elements(pObj, New_trippi_troppi(trippi_troppi_L));

    // initialise monster factory (optional reset)
    MF_Reset();

    // setting derived object function
    pObj->Update = game_scene_update;
    pObj->Draw = game_scene_draw;
    pObj->Destroy = game_scene_destroy;
    return pObj;
}

void game_scene_update(Scene *self)
{
    double now = al_get_time();
    if (_prev_time == 0.0) _prev_time = now;
    double dt = now - _prev_time;
    _prev_time = now;

    // let factory decide whether to spawn monsters this frame
    MF_Update(self, dt);

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
}

void game_scene_destroy(Scene *self)
{
    MF_Destroy(); // clean up factory‑related resources

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
