#include "mfl_tier2.h"
/*005de2fc*/ extern float k_f_0_04;
/*006a9588*/ extern undefined g_weapon_6a9588;


/* weapon_flamethrower_ammo_fraction */

double weapon_flamethrower_ammo_fraction(WeaponSlot *slot)

{
  int record_offset;
  char *record;

  record_offset = slot->player * 0x68;
  record = (char *)&g_weapon_6a9588 + record_offset;
  return (double)((5 - *(int *)(record + 0x14)) * 5 -
                  *(int *)(record + 0x18)) * (double)k_f_0_04;
}
