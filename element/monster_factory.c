#include <allegro5/allegro.h>
#include "monster_factory.h"
#include "../element/tungtungtung.h"
#include "../element/susu.h"
#include "../element/ball.h"
#include "../element/element.h"

/* ---------------------------------------------------------
   Internal control structure
   --------------------------------------------------------- */
typedef struct {
    double  timer;      /* countdown to next wave */
    int     wave_idx;   /* which scripted wave we’re on (reserved) */
} SpawnCtrl;

static SpawnCtrl _ctrl;

/* ---------------------------------------------------------
   Helper: create one monster instance
   --------------------------------------------------------- */
static Elements *create_monster(MonsterType type, float x, float y)
{
    switch (type) {
        case MON_TUNGTUNGTUNG:
            return New_tungtungtung(tungtungtung_L); // position handled inside
        case MON_SUSU:
            return New_susu(Susu_L);
        case MON_BALL:
            return New_Ball(Ball_L);
        default:
            return NULL;
    }
}

/* ---------------------------------------------------------
   Helper: count current tungtungtung in scene
   --------------------------------------------------------- */
static int count_tung(Scene *scene)
{
    int cnt = 0;
    ElementVec all = _Get_all_elements(scene);
    for (int i = 0; i < all.len; ++i)
    {
        if (all.arr[i]->label == tungtungtung_L)
            ++cnt;
    }
    return cnt;
}

/* ---------------------------------------------------------
   Public API
   --------------------------------------------------------- */
void MF_Spawn(Scene *scene, MonsterType type, float x, float y)
{
    Elements *m = create_monster(type, x, y);
    if (m)
        _Register_elements(scene, m);
}

void MF_Update(Scene *scene, double dt)
{
    /* 每 5 秒試圖產生一波怪 */
    _ctrl.timer -= dt;
    if (_ctrl.timer <= 0.0) {
        int curTung = count_tung(scene);
        if (curTung < 10) {
            /* 計算還能再生多少隻，最多補到 10 隻為止 */
            int spawnN = 3;
            if (curTung + spawnN > 10)
                spawnN = 10 - curTung;
            for (int i = 0; i < spawnN; ++i)
                MF_Spawn(scene, MON_TUNGTUNGTUNG, 0, 0);
        }
        /* 始終間隔 5 秒檢查一次 (即使沒生成也重置計時) */
        _ctrl.timer = 5.0;
    }
}

void MF_Reset(void)
{
    _ctrl.timer    = 0.0;
    _ctrl.wave_idx = 0;
}

void MF_Destroy(void)
{
    /* 如果未來配置資源，這裡釋放 */
}
