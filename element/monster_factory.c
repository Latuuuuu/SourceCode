#include <allegro5/allegro.h>
#include "monster_factory.h"
#include "../element/tungtungtung.h"
#include "../element/susu.h"
#include "../element/ball.h"
#include "../element/trippi_troppi.h"
#include "../element/element.h"

/* ---------------------------------------------------------
   Internal control structure
   --------------------------------------------------------- */
typedef struct {
    double  wave_timer; /* 距離下一波刷新的剩餘秒數 */
    int     wave_idx;   /* 保留：腳本波次 (未用)    */
} SpawnCtrl;

static SpawnCtrl _ctrl; /* 單例 */

/* ---------------------------------------------------------
   Helper: 建立單一怪物
   --------------------------------------------------------- */
static Elements *create_monster(MonsterType type, float x, float y)
{
    
    switch (type) {
        case MON_TUNGTUNGTUNG:  return New_tungtungtung(tungtungtung_L);
        case MON_TRIPPI_TROPPI: return New_trippi_troppi(trippi_troppi_L);
        case MON_BALL:          return New_Ball(Ball_L);
        case MON_SUSU:          return New_susu(Susu_L);
        default:                return NULL;
    }
}

/* ---------------------------------------------------------
   Helper: 統計場上存活的主要怪物 (tung & trippi)
   --------------------------------------------------------- */
static int count_alive_monsters(Scene *scene)
{
    int cnt = 0;
    ElementVec all = _Get_all_elements(scene);
    for (int i = 0; i < all.len; ++i) {
        switch (all.arr[i]->label) {
            case tungtungtung_L:
            case trippi_troppi_L:
                ++cnt;
                break;
            default:
                break;
        }
    }
    return cnt;
}

/* ---------------------------------------------------------
   Public API: 外部呼叫
   --------------------------------------------------------- */
void MF_Spawn(Scene *scene, MonsterType type, float x, float y)
{
    Elements *m = create_monster(type, x, y);
    if (m) _Register_elements(scene, m);
}

void MF_Update(Scene *scene, double dt)
{
    const int   TUNG_NUM   = 5;
    const int   TRIPPI_NUM = 3;
    const float INTERVAL   = 2.0f; /* 清場後 2 秒再刷 */

    int alive = count_alive_monsters(scene);

    if (alive == 0) {
        /* 場上沒有 tung / trippi ，開始倒數 */
        _ctrl.wave_timer -= dt;
        if (_ctrl.wave_timer <= 0.0f) {
            /* 產生新一波 */
            for (int i = 0; i < TUNG_NUM; ++i)
                MF_Spawn(scene, MON_TUNGTUNGTUNG, 0, 0);
            for (int i = 0; i < TRIPPI_NUM; ++i)
                MF_Spawn(scene, MON_TRIPPI_TROPPI, 0, 0);
            /* 重置倒數以備下次 */
            _ctrl.wave_timer = INTERVAL;
        }
    } else {
        /* 還有怪 → 維持倒數在 INTERVAL */
        _ctrl.wave_timer = INTERVAL;
    }
}

void MF_Reset(void)
{
    _ctrl.wave_timer = 0.0; /* 啟動時立即刷第一波 */
    _ctrl.wave_idx   = 0;
}

void MF_Destroy(void)
{
    /* 未配置資源，留空 */
}
