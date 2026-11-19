#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
/*005e64a4*/ extern float k_f_0;
/*006a7c78*/ extern undefined g_weapon_6a7c78;


/* weapon_oil_ammo_fraction */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

double weapon_oil_ammo_fraction(WeaponSlot *slot)

{
  float *ammo;
  
  ammo = (float *)(&g_weapon_6a7c78 + slot->player * 0x10);
  if (*ammo < k_f_0) {
    return (double)k_f_0;
  }
  if (*ammo > float_1_0) {
    return (double)float_1_0;
  }
  return (double)*ammo;
}
