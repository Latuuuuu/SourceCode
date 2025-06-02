#ifndef DAMAGEABLE_H
#define DAMAGEABLE_H
#include "../shapes/Shape.h"

typedef struct{
    int side; // 0 for susu, 1 for moster
    int hp;
    Shape *hitbox;
} Damageable;

#endif /* DAMAGEABLE_H */
