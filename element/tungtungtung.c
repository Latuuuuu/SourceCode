#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>
#include "tungtungtung.h"
#include "susu.h"              /* 提供 get_susu() 介面 */
#include "../scene/sceneManager.h"
#include "../shapes/Rectangle.h"
#include "../global.h"           /* WIDTH / HEIGHT 常數 */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* --------------------------------------------------
   追蹤行為參數
   --------------------------------------------------*/
#define CHASE_SPEED    3.0f  /* 像素 / frame */
#define ARRIVE_EPSILON 60.0f  /* 抵達判定半徑 */

/* --------------------------------------------------
   建構函式
   --------------------------------------------------*/
Elements *New_tungtungtung(int label)
{
    tungtungtung *pDerivedObj = malloc(sizeof(tungtungtung));
    Elements     *pObj        = New_Elements(label);

    /* 載入靜態貼圖 */
    const char *state_string[3] = {"stop", "move", "attack"};
    for (int i = 0; i < 3; ++i) {
        char buffer[64];
        sprintf(buffer, "assets/image/tungtungtung_%s.png", state_string[i]);
        pDerivedObj->img[i] = al_load_bitmap(buffer);
    }

    /* 幾何資料 */
    pDerivedObj->width  = al_get_bitmap_width (pDerivedObj->img[0]);
    pDerivedObj->height = al_get_bitmap_height(pDerivedObj->img[0]);
    
    Elements *susu_elem = get_susu();
    susu * player =NULL;
    if(susu_elem) player = (susu*)susu_elem->pDerivedObj;
    do{
        pDerivedObj->x = rand()%(WIDTH-pDerivedObj->width);                                  /* 起始座標 */
        pDerivedObj->y = rand()%(HEIGHT-pDerivedObj->height);
    }while(susu_elem && abs(pDerivedObj->x - player->x) < ARRIVE_EPSILON&&abs(pDerivedObj->y - player->y) < ARRIVE_EPSILON);
    
    pDerivedObj->hitbox = New_Rectangle(
        pDerivedObj->x,
        pDerivedObj->y,
        pDerivedObj->x + pDerivedObj->width,
        pDerivedObj->y + pDerivedObj->height);

    pDerivedObj->dir   = false;  /* 預設面向左 */
    pDerivedObj->state = STOP;

    /* 綁定多型函式 */
    pObj->pDerivedObj = pDerivedObj;
    pObj->Draw        = tungtungtung_draw;
    pObj->Update      = tungtungtung_update;
    pObj->Interact    = tungtungtung_interact;
    pObj->Destroy     = tungtungtung_destory;

    return pObj;
}

/* --------------------------------------------------
   每幀更新：固定速率追蹤 susu
   --------------------------------------------------*/
void tungtungtung_update(Elements *self)
{
    tungtungtung *chara = self->pDerivedObj;

    /* 透過單例 accessor 取得 susu */
    Elements *susu_elem = get_susu();
    if (!susu_elem) return;              /* 還沒生成 susu */

    susu *target = susu_elem->pDerivedObj;

    /* 1) 取得雙方中心點 */
    int cx = chara->x + chara->width  / 2;
    int cy = chara->y + chara->height / 2;
    int tx = target->x + target->width  / 2;
    int ty = target->y + target->height / 2;

    int dx = tx - cx;
    int dy = ty - cy;
    float dist = sqrtf((float)dx * dx + (float)dy * dy);

    /* 2) 移動或停止 */
    if (dist > ARRIVE_EPSILON) {
        float vx = CHASE_SPEED * dx / dist;
        float vy = CHASE_SPEED * dy / dist;
        _tungtungtung_update_position(self, (int)vx, (int)vy);
        chara->dir   = (dx >= 0);
        chara->state = MOVE;
    } else {
        chara->state = STOP;
    }
}

/* --------------------------------------------------
   繪圖
   --------------------------------------------------*/
void tungtungtung_draw(Elements *self)
{
    tungtungtung *chara = self->pDerivedObj;
    ALLEGRO_BITMAP *bmp = chara->img[chara->state];
    if (!bmp) return;

    al_draw_bitmap(bmp,
                   chara->x,
                   chara->y,
                   chara->dir ? ALLEGRO_FLIP_HORIZONTAL : 0);
}

void tungtungtung_interact(Elements *self) { /* 之後可以處理碰撞 */ }

/* --------------------------------------------------
   清理
   --------------------------------------------------*/
void tungtungtung_destory(Elements *self)
{
    tungtungtung *chara = self->pDerivedObj;
    for (int i = 0; i < 3; ++i)
        if (chara->img[i]) al_destroy_bitmap(chara->img[i]);
    free(chara->hitbox);
    free(chara);
    free(self);
}

/* --------------------------------------------------
   私有：同步位置與 hitbox，並限制在視窗內
   --------------------------------------------------*/
void _tungtungtung_update_position(Elements *self, int dx, int dy)
{
    tungtungtung *chara = self->pDerivedObj;

    chara->x += dx;
    chara->y += dy;

    /* 保持在畫面邊界 */
    if (chara->x < 0)                       chara->x = 0;
    if (chara->y < 0)                       chara->y = 0;
    if (chara->x > WIDTH  - chara->width)   chara->x = WIDTH  - chara->width;
    if (chara->y > HEIGHT - chara->height)  chara->y = HEIGHT - chara->height;

    /* hitbox 也要同步 */
    Shape *hb = chara->hitbox;
    hb->update_center_x(hb, dx);
    hb->update_center_y(hb, dy);
}
