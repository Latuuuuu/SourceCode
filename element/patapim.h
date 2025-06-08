#ifndef patapim_H_INCLUDED
#define patapim_H_INCLUDED

#include <allegro5/allegro_audio.h>
#include "element.h"
#include "damageable.h"
#include "../shapes/Shape.h"
#include "../algif5/algif.h"
#include <stdbool.h>
#include "state.h"

/*
   [patapim object]
*/
typedef struct
{
    Damageable base;
    int  x, y;
    int  width, height;             // image size
    bool dir;                       // true: face right
    int  state;                     // STOP / MOVE / ATTACK
    ALLEGRO_BITMAP *img[3];         // 0:stop 1:move 2:attack
    int  anime;                     // (若日後要做動圖可留)
    int  anime_time;
    bool new_proj;                  // 目前沒用，可保留
    int  attack_timer;              // ★ 個別冷卻計時器 (frame)
} patapim;

Elements *New_patapim(int label);
void      patapim_update(Elements *self);
void      patapim_interact(Elements *self);
void      patapim_draw(Elements *self);
void      patapim_destory(Elements *self);
void      _patapim_update_position(Elements *self, int dx, int dy);

#endif /* patapim_H_INCLUDED */
