// cluster-entries: car_damage_update_all
/* Installed cluster car_damage_update_all (tools/cluster_install.py from output/cluster_pilot_zx/car_damage_update_all.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*0065a9e8*/ extern undefined4 car_damage;
extern char g_car_damage_65aa88[];
/* size of one car_damage record in dwords (inferred: stride of the loop below) */
#define CAR_DAMAGE_RECORD_DWORDS (10)

/* car_damage_update_all */

/* "calls the per car damage update (0x424000) for every car_damage record" */

static int car_damage_update_r(float * p0);
extern void car_damage_update(void);
void car_damage_update_all(void)

{
  undefined4 *damage_record;

  damage_record = &car_damage;
  /* walk the car_damage array record by record up to the end marker g_car_damage_65aa88 (inferred) */
  do {
    car_damage_update_r(damage_record);
    damage_record = damage_record + CAR_DAMAGE_RECORD_DWORDS;
  } while ((LONG_PTR)damage_record < ((LONG_PTR)&g_car_damage_65aa88));
  return;
}

/* car_damage_update_r: C -> original car_damage_update (registers esi) */
__declspec(naked) int car_damage_update_r(float * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call car_damage_update
        pop esi
        ret
    }
}
