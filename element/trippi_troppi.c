#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>
#include "trippi_troppi.h"
#include "susu.h"              /* get_susu() */
#include "atk.h"               /* New_Atk()  */
#include "../scene/sceneManager.h"
#include "../shapes/Rectangle.h"
#include "../global.h"
#include <math.h>
#include <stdlib.h> 
#include <stdio.h>
#include <stdbool.h>
#include "../scene/gamescene.h"   /* ⬅️ 提供 Atk_L、tungtungtung_L... */

/* --------------------------------------------------
   追蹤 / 攻擊參數
   --------------------------------------------------*/
#define CHASE_SPEED        3.0f   /* 像素 / frame */
#define ARRIVE_EPSILON    60.0f   /* 停下距離     */
#define COOLDOWN_FRAMES  180      /* 60FPS → 3 秒  */
#define BULLET_DAMAGE      40
#define BULLET_SPEED       15.0f
#define SHOT_RANGE  500.0f

static void _trippi_troppi_update_position(Elements *self, int dx, int dy);

/* --------------------------------------------------
   建構
   --------------------------------------------------*/
Elements *New_trippi_troppi(int label)
{
    trippi_troppi *pDerivedObj = malloc(sizeof(trippi_troppi));
    Elements      *pObj        = New_Elements(label);

    /* 載入三張 PNG：停 / 走 / 攻 (可共用攻擊圖或重複最後一張) */
    const char *state_str[3] = {"stop", "move", "attack"};
    for (int i = 0; i < 3; ++i) {
        char buf[64];
        sprintf(buf, "assets/image/Trippi-Troppi_%s.png", state_str[i]);
        pDerivedObj->img[i] = al_load_bitmap(buf);
    }

    /* 幾何 */
    pDerivedObj->width  = al_get_bitmap_width (pDerivedObj->img[0]);
    pDerivedObj->height = al_get_bitmap_height(pDerivedObj->img[0]);

    /* 隨機初始位置，避免貼玩家 */
    Elements *plE = get_susu();
    susu *pl = plE ? (susu*)plE->pDerivedObj : NULL;
    do {
        pDerivedObj->x = rand() % (WIDTH  - pDerivedObj->width);
        pDerivedObj->y = rand() % (HEIGHT - pDerivedObj->height);
    } while (pl && fabs(pDerivedObj->x - pl->x) < ARRIVE_EPSILON &&
                  fabs(pDerivedObj->y - pl->y) < ARRIVE_EPSILON);

    /* Damageable 基底 */
    pDerivedObj->base.hp    = 60;
    pDerivedObj->base.side  = 1; /* 敵方 */
    pDerivedObj->base.hitbox = New_Rectangle(
        pDerivedObj->x,
        pDerivedObj->y,
        pDerivedObj->x + pDerivedObj->width,
        pDerivedObj->y + pDerivedObj->height);

    pDerivedObj->dir      = false;
    pDerivedObj->state    = STOP;
    pDerivedObj->cooldown = 0;   /* 立即可射擊 */

    /* 綁定函式 */
    pObj->pDerivedObj = pDerivedObj;
    pObj->Draw        = trippi_troppi_draw;
    pObj->Update      = trippi_troppi_update;
    pObj->Interact    = trippi_troppi_interact;
    pObj->Destroy     = trippi_troppi_destory;

    return pObj;
}

/* --------------------------------------------------
   更新：追蹤 + 冷卻射擊
   --------------------------------------------------*/
void trippi_troppi_update(Elements *self)
{
    trippi_troppi *ch = self->pDerivedObj;
    if (ch->cooldown > 0) ch->cooldown--; /* 倒數 */

    Elements *plE = get_susu();
    if (!plE) return;
    susu *pl = plE->pDerivedObj;

    /* 中心點 */
    int cx = ch->x + ch->width  / 2;
    int cy = ch->y + ch->height / 2;
    int tx = pl->x + pl->width  / 2;
    int ty = pl->y + pl->height / 2;
    int dx = tx - cx;
    int dy = ty - cy;
    float dist = sqrtf((float)dx * dx + (float)dy * dy);

    /* 追蹤移動 */
    if (dist > SHOT_RANGE) {
        float vx = CHASE_SPEED * dx / dist;
        float vy = CHASE_SPEED * dy / dist;
        _trippi_troppi_update_position(self, (int)vx, (int)vy);
        ch->dir   = (dx >= 0);
        ch->state = MOVE;
    } else {
        ch->state = STOP;
    }

    /* 射擊判定：只有在 STOP 狀態且冷卻完畢 */
    if (ch->state == STOP && ch->cooldown == 0) {
        if (dist < 1.0f) dist = 1.0f;
        float vx = BULLET_SPEED * dx / dist;
        float vy = BULLET_SPEED * dy / dist;
        Elements *proj = New_Atk(Atk_L,
                                 cx - 20.0f, cy - 20.0f,
                                 vx, vy,
                                 BULLET_DAMAGE,
                                 1); /* 怪物 side */
        if (proj) _Register_elements(scene, proj);
        ch->cooldown = COOLDOWN_FRAMES;
    }
}

/* --------------------------------------------------
   繪圖
   --------------------------------------------------*/
void trippi_troppi_draw(Elements *self)
{
    trippi_troppi *ch = self->pDerivedObj;
    ALLEGRO_BITMAP *bmp = ch->img[ch->state];
    if (!bmp) return;
    al_draw_bitmap(bmp, ch->x, ch->y, ch->dir ? ALLEGRO_FLIP_HORIZONTAL : 0);
}

void trippi_troppi_interact(Elements *self) { /* 如需碰撞處理可擴充 */ }

/* --------------------------------------------------
   清理
   --------------------------------------------------*/
void trippi_troppi_destory(Elements *self)
{
    trippi_troppi *ch = self->pDerivedObj;
    for (int i = 0; i < 3; ++i)
        if (ch->img[i]) al_destroy_bitmap(ch->img[i]);
    free(ch->base.hitbox);
    free(ch);
    free(self);
}

/* --------------------------------------------------
   私用：位置與 hitbox 同步
   --------------------------------------------------*/
static void _trippi_troppi_update_position(Elements *self, int dx, int dy)
{
    trippi_troppi *ch = self->pDerivedObj;
    ch->x += dx;
    ch->y += dy;

    if (ch->x < 0)                       ch->x = 0;
    if (ch->y < 0)                       ch->y = 0;
    if (ch->x > WIDTH  - ch->width)      ch->x = WIDTH  - ch->width;
    if (ch->y > HEIGHT - ch->height)     ch->y = HEIGHT - ch->height;

    Shape *hb = ch->base.hitbox;
    hb->update_center_x(hb, dx);
    hb->update_center_y(hb, dy);
}