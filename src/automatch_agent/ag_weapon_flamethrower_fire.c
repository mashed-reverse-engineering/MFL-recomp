#include "mfl_tier2.h"
/*006a9588*/ extern undefined g_weapon_6a9588;


/* weapon_flamethrower_fire */

void weapon_flamethrower_fire(WeaponSlot *slot,int param_2)

{
  int *flame_record;

  flame_record = (int *)(&g_weapon_6a9588 + slot->player * 0x68);
  if (flame_record[5] >= 5) {
    flame_record[7] = 0;
    flame_record[0] = 0;
    flame_record[5] = 5;
    return;
  }
  if (param_2 != 1) {
    flame_record[7] = 1;
  }
  return;
}
