#include "Atk.h"
#include "damageable.h"
#include "tree.h"
#include "../shapes/Circle.h"
#include "../scene/gamescene.h"   /* for element label */
#include "../scene/sceneManager.h" /* for scene variable */

/* --------------------------------------------------
   Internal helpers declared in Atk.h (no static here)
   --------------------------------------------------*/
void _Atk_update_position(Elements *self, float dx, float dy);
void _Atk_interact_Floor(Elements *self, Elements *tar);
void _Atk_interact_Tree (Elements *self, Elements *tar);
void DealDamageIfPossible(Elements *target, int damage);

/* --------------------------------------------------
   Constructor
   --------------------------------------------------*/
Elements *New_Atk(int label, int x, int y, float vx, float vy, int damage, int side)
{
    Atk *pDerivedObj = (Atk *)malloc(sizeof(Atk));
    Elements *pObj   = New_Elements(label);

    /* graphics */
    pDerivedObj->img    = al_load_bitmap("assets/image/ball-02.png");
    pDerivedObj->width  = al_get_bitmap_width (pDerivedObj->img);
    pDerivedObj->height = al_get_bitmap_height(pDerivedObj->img);

    /* physics */
    pDerivedObj->x      = x;
    pDerivedObj->y      = y;
    pDerivedObj->vx     = vx;
    pDerivedObj->vy     = vy;
    pDerivedObj->damage = damage;
    pDerivedObj->side   = side;

    /* circular hit-box */
    pDerivedObj->hitbox = New_Circle(
        pDerivedObj->x + pDerivedObj->width  / 2,
        pDerivedObj->y + pDerivedObj->height / 2,
        min(pDerivedObj->width, pDerivedObj->height) / 2);

    /* interaction whitelist */
    pObj->inter_obj[pObj->inter_len++] = Tree_L;
    pObj->inter_obj[pObj->inter_len++] = Floor_L;
    pObj->inter_obj[pObj->inter_len++] = tungtungtung_L;
    pObj->inter_obj[pObj->inter_len++] = Susu_L;
    pObj->inter_obj[pObj->inter_len++] = trippi_troppi_L;

    /* virtual methods */
    pObj->pDerivedObj = pDerivedObj;
    pObj->Update      = Atk_update;
    pObj->Interact    = Atk_interact;
    pObj->Draw        = Atk_draw;
    pObj->Destroy     = Atk_destory;

    return pObj;
}

/* --------------------------------------------------
   Position + hit-box sync (Circle expects Δ)
   --------------------------------------------------*/
void _Atk_update_position(Elements *self, float dx, float dy)
{
    Atk *atk = (Atk *)self->pDerivedObj;

    /* 1. save old position */
    int old_x = atk->x;
    int old_y = atk->y;

    /* 2. move */
    atk->x += dx;
    atk->y += dy;

    /* 3. delete if far off‑screen */
    if (atk->x < -atk->width  * 2 || atk->x > WIDTH  + atk->width  * 2 ||
        atk->y < -atk->height * 2 || atk->y > HEIGHT + atk->height * 2) {
        self->dele = true;
        return;
    }

    /* 4. sync hit‑box with true delta */
    int real_dx = atk->x - old_x;
    int real_dy = atk->y - old_y;

    Shape *hb = atk->hitbox;
    hb->update_center_x(hb, real_dx);
    hb->update_center_y(hb, real_dy);
}

/* --------------------------------------------------
   Per‑frame update
   --------------------------------------------------*/
void Atk_update(Elements *self)
{
    Atk *atk = (Atk *)self->pDerivedObj;
    _Atk_update_position(self, atk->vx, atk->vy);
}

/* --------------------------------------------------
   Collision checks
   --------------------------------------------------*/
void Atk_interact(Elements *self)
{
    Atk *atk = (Atk *)self->pDerivedObj;

    for (int j = 0; j < self->inter_len; ++j) {
        int inter_label = self->inter_obj[j];
        ElementVec labelEle = _Get_label_elements(scene, inter_label);
        for (int i = 0; i < labelEle.len; ++i) {
            Elements *tar = labelEle.arr[i];
            if (!tar || !tar->pDerivedObj) continue;

            Damageable *dmg = (Damageable *)tar->pDerivedObj;
            Shape *tar_hitbox = dmg->hitbox;
            if (!tar_hitbox) continue;

            if (tar_hitbox->overlap(tar_hitbox, atk->hitbox) && atk->side != dmg->side) {
                DealDamageIfPossible(tar, atk->damage);
                self->dele = true;
                return;
            }
        }
    }
}

/* --------------------------------------------------
   Placeholder helpers (reserved for FX)
   --------------------------------------------------*/
void _Atk_interact_Floor(Elements *self, Elements *tar) {}
void _Atk_interact_Tree (Elements *self, Elements *tar) {}

/* --------------------------------------------------
   Rendering
   --------------------------------------------------*/
void Atk_draw(Elements *self)
{
    if (self->dele) return; /* already flagged for removal */
    Atk *atk = (Atk *)self->pDerivedObj;
    int flags = (atk->vx > 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;
    al_draw_bitmap(atk->img, atk->x, atk->y, flags);
}

/* --------------------------------------------------
   Destructor
   --------------------------------------------------*/
void Atk_destory(Elements *self)
{
    if (!self) return;
    Atk *atk = (Atk *)self->pDerivedObj;
    if (atk) {
        if (atk->img)    al_destroy_bitmap(atk->img);
        if (atk->hitbox) free(atk->hitbox);
        free(atk);
    }
    free(self);
}

/* --------------------------------------------------
   Damage helper
   --------------------------------------------------*/
void DealDamageIfPossible(Elements *target, int damage)
{
    if (!target || !target->pDerivedObj) return;
    Damageable *dmg = (Damageable *)target->pDerivedObj;
    if (!dmg->hitbox) return;

    dmg->hp -= damage;
    printf("Hit! remaining hp = %d\n", dmg->hp);

    if (dmg->hp <= 0) target->dele = true;
}
