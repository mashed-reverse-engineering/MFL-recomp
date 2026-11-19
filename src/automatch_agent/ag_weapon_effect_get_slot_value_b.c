#include "mfl_tier2.h"
/*006a9428*/ extern undefined4 g_weapon_6a9428;


/* weapon_effect_get_slot_value_b */

/* "(player 0..3): g_weapon_6a9428[player * 0x58] or -1" */

undefined4 weapon_effect_get_slot_value_b(int param_1)

{
  if ((param_1 >= 0) && (param_1 < 4)) {
    return (&g_weapon_6a9428)[param_1 * 0x16];
  }
  return 0xffffffff;
}
