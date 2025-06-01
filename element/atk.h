#ifndef Atk_H_INCLUDED
#define Atk_H_INCLUDED
#include "element.h"
#include "../shapes/Shape.h"
/*
   [Atk object]
*/
typedef struct _Atk
{
    int x, y;          // the position of image
    int width, height; // the width and height of image
    int vx,vy;             // the velocity of Atk
    ALLEGRO_BITMAP *img;
    Shape *hitbox; // the hitbox of object
} Atk;
Elements *New_Atk(int label, int x, int y, float vx, float vy);
void Atk_update(Elements *self);
void Atk_interact(Elements *self);
void Atk_draw(Elements *self);
void Atk_destory(Elements *self);
void _Atk_update_position(Elements *self, float dx, float dy);
void _Atk_interact_Floor(Elements *self, Elements *tar);
void _Atk_interact_Tree(Elements *self, Elements *tar);

#endif
