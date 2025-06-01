#include "Atk.h"
#include "tree.h"
#include "../shapes/Circle.h"
#include "../scene/gamescene.h" // for element label
#include "../scene/sceneManager.h" // for scene variable
/*
   [Atk function]
*/
Elements *New_Atk(int label, int x, int y, float vx,float vy)
{
    Atk *pDerivedObj = (Atk *)malloc(sizeof(Atk));
    Elements *pObj = New_Elements(label);
    // setting derived object member
    pDerivedObj->img = al_load_bitmap("assets/image/projectile.png");
    pDerivedObj->width = al_get_bitmap_width(pDerivedObj->img);
    pDerivedObj->height = al_get_bitmap_height(pDerivedObj->img);
    pDerivedObj->x = x;
    pDerivedObj->y = y;
    pDerivedObj->vx = vx;
    pDerivedObj->vy = vy;
    pDerivedObj->hitbox = New_Circle(pDerivedObj->x + pDerivedObj->width / 2,
                                     pDerivedObj->y + pDerivedObj->height / 2,
                                     min(pDerivedObj->width, pDerivedObj->height) / 2);
    // setting the interact object
    pObj->inter_obj[pObj->inter_len++] = Tree_L;
    pObj->inter_obj[pObj->inter_len++] = Floor_L;
    // setting derived object function
    pObj->pDerivedObj = pDerivedObj;
    pObj->Update = Atk_update;
    pObj->Interact = Atk_interact;
    pObj->Draw = Atk_draw;
    pObj->Destroy = Atk_destory;

    return pObj;
}
void Atk_update(Elements *self)
{
    Atk *Obj = ((Atk *)(self->pDerivedObj));
    _Atk_update_position(self, Obj->vx, Obj->vy);
}
void _Atk_update_position(Elements *self, float dx, float dy)
{
    Atk *Obj = ((Atk *)(self->pDerivedObj));
    Obj->x += dx;
    Obj->y += dy;
    Shape *hitbox = Obj->hitbox;
    hitbox->update_center_x(hitbox, dx);
    hitbox->update_center_y(hitbox, dy);
}
void Atk_interact(Elements *self)
{
    for (int j = 0; j < self->inter_len; j++)
    {
        int inter_label = self->inter_obj[j];
        ElementVec labelEle = _Get_label_elements(scene, inter_label);
        for (int i = 0; i < labelEle.len; i++)
        {
            if (inter_label == Floor_L)
            {
                _Atk_interact_Floor(self, labelEle.arr[i]);
            }
            else if (inter_label == Tree_L)
            {
                _Atk_interact_Tree(self, labelEle.arr[i]);
            }
        }
    }
}
void _Atk_interact_Floor(Elements *self, Elements *tar)
{
    Atk *Obj = ((Atk *)(self->pDerivedObj));
    if (Obj->x < 0 - Obj->width)
        self->dele = true;
    else if (Obj->x > WIDTH + Obj->width)
        self->dele = true;
}
void _Atk_interact_Tree(Elements *self, Elements *tar)
{
    Atk *Obj = ((Atk *)(self->pDerivedObj));
    Tree *tree = ((Tree *)(tar->pDerivedObj));
    if (tree->hitbox->overlap(tree->hitbox, Obj->hitbox))
    {
        self->dele = true;
    }
}
void Atk_draw(Elements *self)
{
    Atk *Obj = ((Atk *)(self->pDerivedObj));
    if (Obj->vx > 0)
        al_draw_bitmap(Obj->img, Obj->x, Obj->y, ALLEGRO_FLIP_HORIZONTAL);
    else
        al_draw_bitmap(Obj->img, Obj->x, Obj->y, 0);
}
void Atk_destory(Elements *self)
{
    Atk *Obj = ((Atk *)(self->pDerivedObj));
    al_destroy_bitmap(Obj->img);
    free(Obj->hitbox);
    free(Obj);
    free(self);
}
