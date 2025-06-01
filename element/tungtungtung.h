#ifndef tungtungtung_H_INCLUDED
#define tungtungtung_H_INCLUDED
#include <allegro5/allegro_audio.h>
#include "element.h"
#include "damageable.h"
#include "../shapes/Shape.h"
#include "../algif5/algif.h"
#include <stdbool.h>
#include "state.h"

/*
   [tungtungtung object]
*/

typedef struct //_tungtungtung
{
    Damageable base;
    int x, y;
    int width, height;              // the width and height of image
    bool dir;                       // true: face to right, false: face to left
    int state;                      // the state of tungtungtung
    ALLEGRO_BITMAP  *img[3]; // png for each state. 0: stop, 1: move, 2:attack
    int anime;      // counting the time of animation
    int anime_time; // indicate how long the animation
    bool new_proj;
    //Shape *hitbox; // the hitbox of object
} tungtungtung;
Elements *New_tungtungtung(int label);
void tungtungtung_update(Elements *self);
void tungtungtung_interact(Elements *self);
void tungtungtung_draw(Elements *self);
void tungtungtung_destory(Elements *self);
void _tungtungtung_update_position(Elements *self, int dx, int dy);

#endif
