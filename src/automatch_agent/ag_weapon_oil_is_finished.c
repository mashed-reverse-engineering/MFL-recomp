#include "mfl_tier2.h"
/*005e64a4*/ extern float k_f_0;
/*006a7c78*/ extern undefined g_weapon_6a7c78;


/* weapon_oil_is_finished */

undefined4 weapon_oil_is_finished(WeaponSlot *param_1)

{
  if (*(float *)(&g_weapon_6a7c78 + param_1->player * 0x10) <= k_f_0) {
    return 1;
  }
  return 0;
}
