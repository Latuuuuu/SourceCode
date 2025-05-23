#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "susu.h"
#include "projectile.h"
#include "../scene/sceneManager.h"
#include "../shapes/Rectangle.h"
#include "../algif5/algif.h"
#include "../scene/gamescene.h"
#include <allegro5/allegro_native_dialog.h>
#include <stdio.h>
#include <stdbool.h>
/*
   [susu function]
*/
Elements *New_susu(int label)
{
    susu *pDerivedObj = (susu *)malloc(sizeof(susu));
    Elements *pObj = New_Elements(label);
    // setting derived object member
    // load susu images
    /*char state_string[3][10] = {"susu_stop", "susu_move", "attack"};
    for (int i = 0; i < 3; i++)
    {
        char buffer[50];
        sprintf(buffer, "assets/image/susu.png");
        
        pDerivedObj->gif_status[i] = algif_new_gif(buffer, -1);
    }*/
    pDerivedObj->img = al_load_bitmap("assets/image/susu_1.png");
    // load effective sound
    ALLEGRO_SAMPLE *sample = al_load_sample("assets/sound/atk_sound.wav");
    pDerivedObj->atk_Sound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(pDerivedObj->atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(pDerivedObj->atk_Sound, al_get_default_mixer());

    // initial the geometric information of susu
    pDerivedObj->width = al_get_bitmap_width(pDerivedObj->img);
    pDerivedObj->height = al_get_bitmap_height(pDerivedObj->img);
    pDerivedObj->x = 300;
    pDerivedObj->y = HEIGHT - pDerivedObj->height - 60;
    pDerivedObj->hitbox = New_Rectangle(pDerivedObj->x,
                                        pDerivedObj->y,
                                        pDerivedObj->x + pDerivedObj->width,
                                        pDerivedObj->y + pDerivedObj->height);
    pDerivedObj->dir = false; // true: face to right, false: face to left
    // initial the animation component
    pDerivedObj->state = susu_STOP;
    pDerivedObj->new_proj = false;
    pObj->pDerivedObj = pDerivedObj;
    // setting derived object function
    pObj->Draw = susu_draw;
    pObj->Update = susu_update;
    pObj->Interact = susu_interact;
    pObj->Destroy = susu_destory;
    return pObj;
}
void susu_update(Elements *self)
{
    // use the idea of finite state machine to deal with different state
    susu *chara = ((susu *)(self->pDerivedObj));
    if (chara->state == susu_STOP)
    {
        if (key_state[ALLEGRO_KEY_SPACE])
        {
            chara->state = susu_ATK;
        }
        else if (key_state[ALLEGRO_KEY_A])
        {
            chara->dir = 0;
            chara->state = susu_MOVE;
        }
        else if (key_state[ALLEGRO_KEY_D])
        {
            chara->dir = 1;
            chara->state = susu_MOVE;
        }
        else if (key_state[ALLEGRO_KEY_W])
        {
            chara->dir = 2;
            chara->state = susu_MOVE;
        }
        else if (key_state[ALLEGRO_KEY_S])
        {
            chara->dir = 3;
            chara->state = susu_MOVE;
        }
        else
        {
            chara->state = susu_STOP;
        }
    }
    else if (chara->state == susu_MOVE)
    {
        if (key_state[ALLEGRO_KEY_SPACE])
        {
            chara->state = susu_ATK;
        }
        else if (key_state[ALLEGRO_KEY_A])
        {
            chara->dir = 0;
            _susu_update_position(self, -5, 0);
            chara->state = susu_MOVE;
        }
        else if (key_state[ALLEGRO_KEY_D])
        {
            chara->dir = 1;
            _susu_update_position(self, 5, 0);
            chara->state = susu_MOVE;
        }
        else if (key_state[ALLEGRO_KEY_W])
        {
            chara->dir = 2;
            _susu_update_position(self, 0, -5);
            chara->state = susu_MOVE;
        }
        else if (key_state[ALLEGRO_KEY_S])
        {
            chara->dir = 3;
            _susu_update_position(self, 0, 5);
            chara->state = susu_MOVE;
        }
        /*if (chara->gif_status[chara->state]->done)
            chara->state = susu_STOP;*/
    }
    else if (chara->state == susu_ATK)
    {
        /*if (chara->gif_status[chara->state]->done)
        {
            chara->state = susu_STOP;
            chara->new_proj = false;
        }*/
        if (/*chara->gif_status[susu_ATK]->display_index == 2 &&*/ chara->new_proj == false)
        {
            Elements *pro;
            if (chara->dir==1)
            {
                pro = New_Projectile(Projectile_L,
                                     chara->x + chara->width - 100,
                                     chara->y + 10,
                                     5);
            }
            else if(chara->dir ==0)
            {
                pro = New_Projectile(Projectile_L,
                                     chara->x - 50,
                                     chara->y + 10,
                                     -5);
            }
            else
            {
                pro = New_Projectile(Projectile_L,
                                     chara->x + chara->width - 100,
                                     chara->y + 10,
                                     5);
            }
            _Register_elements(scene, pro);
            chara->new_proj = true;
        }
    }
}
void susu_draw(Elements *self)
{
    
    // with the state, draw corresponding image
    susu *chara = ((susu *)(self->pDerivedObj));
    //ALLEGRO_BITMAP *frame = algif_get_bitmap(chara->gif_status[chara->state], al_get_time());
    al_draw_bitmap(chara->img, chara->x, chara->y, ((chara->dir) ? ALLEGRO_FLIP_HORIZONTAL : 0));
    /*if (frame)
    {
        al_draw_bitmap(frame, chara->x, chara->y, ((chara->dir) ? ALLEGRO_FLIP_HORIZONTAL : 0));
    }
    if (chara->state == susu_ATK && chara->gif_status[chara->state]->display_index == 2)
    {
        al_play_sample_instance(chara->atk_Sound);  
    }*/
}
void susu_destory(Elements *self)
{
    susu *Obj = ((susu *)(self->pDerivedObj));
    al_destroy_sample_instance(Obj->atk_Sound);
    al_destroy_bitmap(Obj->img);
    /*for (int i = 0; i < 3; i++)
        algif_destroy_animation(Obj->gif_status[i]);*/
    free(Obj->hitbox);
    free(Obj);
    free(self);
}

void _susu_update_position(Elements *self, int dx, int dy)
{
    susu *chara = ((susu *)(self->pDerivedObj));
    chara->x += dx;
    chara->y += dy;
    Shape *hitbox = chara->hitbox;
    hitbox->update_center_x(hitbox, dx);
    hitbox->update_center_y(hitbox, dy);
}

void susu_interact(Elements *self) {}
