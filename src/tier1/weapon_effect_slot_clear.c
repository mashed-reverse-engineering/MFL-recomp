#include "mfl_tier2.h"
/*006a9428*/ extern undefined4 g_weapon_6a9428;
/*006a9430*/ extern undefined4 g_weapon_6a9430;


/* weapon_effect_slot_clear */

/* "(slot): active = 0, id = -1" */

void weapon_effect_slot_clear(int slot)
{
  (&g_weapon_6a9430)[slot * 0x16] = 0;
  (&g_weapon_6a9428)[slot * 0x16] = 0xffffffff;
  return;
}
