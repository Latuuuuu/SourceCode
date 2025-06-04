#ifndef trippi_troppi_H_INCLUDED
#define trippi_troppi_H_INCLUDED
#include <allegro5/allegro_audio.h>
#include "element.h"
#include "damageable.h"
#include "../shapes/Shape.h"
#include "../algif5/algif.h"
#include <stdbool.h>
#include "state.h"

/*
   [trippi_troppi object]
*/

typedef struct //_trippi_troppi
{
    Damageable base;
    int x, y;
    int width, height;              // the width and height of image
    bool dir;                       // true: face to right, false: face to left
    int state;                      // the state of trippi_troppi
    ALLEGRO_BITMAP  *img[3]; // png for each state. 0: stop, 1: move, 2:attack
    int anime;      // counting the time of animation
    int anime_time; // indicate how long the animation
    bool new_proj;
    Shape *hitbox; // the hitbox of object
} trippi_troppi;

Elements *New_trippi_troppi(int label);
void trippi_troppi_update(Elements *self);
void trippi_troppi_interact(Elements *self);
void trippi_troppi_draw(Elements *self);
void trippi_troppi_destory(Elements *self);
void _trippi_troppi_update_position(Elements *self, int dx, int dy);

#endif
