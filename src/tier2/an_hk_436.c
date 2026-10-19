#include "mfl_tier2.h"
/*0065a9e8*/ extern undefined4 car_damage;
static int car_damage_update_r(float * reg_esi);
extern void car_damage_update(void);
extern char g_car_damage_65aa88[];
/* size of one car_damage record in dwords (inferred: stride of the loop below) */
#define CAR_DAMAGE_RECORD_DWORDS (10)
__declspec(naked) int car_damage_update_r(float * reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call car_damage_update
        pop esi
        ret
    }
}

/* car_damage_update_all */

/* "calls the per car damage update (0x424000) for every car_damage record" */

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


