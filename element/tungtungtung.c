#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>
#include "charater.h"
#include "projectile.h"
#include "tungtungtung.h"
#include "../scene/sceneManager.h"
#include "../shapes/Rectangle.h"
//#include "../algif5/algif.h"
#include "../scene/gamescene.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
/*
   [tungtungtung function]
*/
Elements *New_tungtungtung(int label)
{
    tungtungtung *pDerivedObj = (tungtungtung *)malloc(sizeof(tungtungtung));
    Elements *pObj = New_Elements(label);
    // setting derived object member
    // load tungtungtung images
    char state_string[3][10] = {"stop", "move", "attack"};
    for (int i = 0; i < 3; i++)
    {
        char buffer[50];
        sprintf(buffer, "assets/image/tungtungtung_%s.png", state_string[i]);
        pDerivedObj->img[i] = al_load_bitmap(buffer);
    }

    // initial the geometric information of tungtungtung
    pDerivedObj->width = al_get_bitmap_width (pDerivedObj->img[0]);
    pDerivedObj->height = al_get_bitmap_height(pDerivedObj->img[0]);
    pDerivedObj->x = 300;
    pDerivedObj->y = HEIGHT - pDerivedObj->height - 60;
    pDerivedObj->base.hp=50;
    pDerivedObj->base.hitbox = New_Rectangle(pDerivedObj->x,
                                        pDerivedObj->y,
                                        pDerivedObj->x + pDerivedObj->width,
                                        pDerivedObj->y + pDerivedObj->height);
    pDerivedObj->dir = false; // true: face to right, false: face to left
    // initial the animation component
    pDerivedObj->state = STOP;
    pDerivedObj->new_proj = false;
    pObj->pDerivedObj = pDerivedObj;
    // setting derived object function
    pObj->Draw = tungtungtung_draw;
    pObj->Update = tungtungtung_update;
    pObj->Interact = tungtungtung_interact;
    pObj->Destroy = tungtungtung_destory;
    return pObj;
}
void tungtungtung_update(Elements *self)
{
    // use the idea of finite state machine to deal with different state
    tungtungtung *chara = ((tungtungtung *)(self->pDerivedObj));
  
    int centerX = chara->x + chara->width / 2;
    int centerY = chara->y + chara->height / 2;


    int dx = mouse.x - centerX;
    int dy = mouse.y - centerY;

    float distance = sqrtf(dx * dx + dy * dy);
    if (distance > 3.0f)
    {
        // 計算單位向量方向，乘上移動速度
        float speed = 3.0f;
        float vx = speed * dx / distance;
        float vy = speed * dy / distance;

        // 更新位置
        _tungtungtung_update_position(self, (int)vx, (int)vy);

        // 根據 X 軸方向設定角色面向
        chara->dir = (dx >= 0);

        // 設定狀態為 MOVE，這樣動畫可以播放走路圖
        chara->state = MOVE;
    }
    else
    {
        // 若已經靠近滑鼠，就停止
        chara->state = STOP;
    }
}
void tungtungtung_draw(Elements *self)
{
    tungtungtung *chara = self->pDerivedObj; 
    ALLEGRO_BITMAP *bmp = chara->img[chara->state];
    if (bmp) {
        al_draw_bitmap(bmp, chara->x, chara->y,
                    chara->dir ? ALLEGRO_FLIP_HORIZONTAL : 0);
}
}
void tungtungtung_destory(Elements *self)
{
    tungtungtung *Obj = ((tungtungtung *)(self->pDerivedObj));
    for (int i = 0; i < 3; i++)
        if (Obj->img[i]) al_destroy_bitmap(Obj->img[i]);
    free(Obj->base.hitbox);
    free(Obj);
    free(self);
}

void _tungtungtung_update_position(Elements *self, int dx, int dy)
{
    tungtungtung *chara = ((tungtungtung *)(self->pDerivedObj));
    chara->x += dx;
    chara->y += dy;
    Shape *hitbox = chara->base.hitbox;
    hitbox->update_center_x(hitbox, dx);
    hitbox->update_center_y(hitbox, dy);
}

void tungtungtung_interact(Elements *self) {}
