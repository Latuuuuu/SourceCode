#ifndef DAMAGEABLE_H
#define DAMAGEABLE_H
#include "../shapes/Shape.h"

typedef struct{
    int hp;
    Shape *hitbox;
} Damageable;

#endif /* DAMAGEABLE_H */
