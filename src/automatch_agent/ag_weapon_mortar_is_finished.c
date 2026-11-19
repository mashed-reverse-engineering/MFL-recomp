#include "mfl_tier2.h"
/*005e64a4*/ extern float k_f_0;
/*006a29b8*/ extern undefined g_weapon_mortar_6a29b8;


/* weapon_mortar_is_finished */

undefined4 weapon_mortar_is_finished(WeaponSlot *param_1)

{
  char *base;
  int cnt;
  
  base = (char *)&g_weapon_mortar_6a29b8 + param_1->player * 0x1c;
  cnt = *(int *)(base + 0xc);
  if (cnt > 0) {
    return 0;
  }
  if (*(float *)(base + 0x10) <= k_f_0) {
    return 1;
  }
  return 0;
}
