#include "mfl_tier2.h"
/*00467e90*/ extern void * course_get_rpworld_00467e90();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*006a29b8*/ extern undefined4 g_weapon_6a29b8;
/*006a29c0*/ extern undefined4 g_weapon_6a29c0;
/*006a29cc*/ extern undefined4 g_weapon_6a29cc;
/* Per-player stride, in 4-byte words, of the g_weapon_6a29c0 / g_weapon_6a29cc arrays (inferred) */
#define PLAYER_GLOBAL_STRIDE (7)
/*
 * Unequips the mortar weapon from the player that owns `slot`.
 * Clears that player's mortar-active flag (inferred), clears the flag on the atomics of
 * the player's mortar clump, and removes the clump from the course's RpWorld.
 * Returns nothing.
 */
void weapon_mortar_unequip(WeaponSlot *slot)

{
  char *rec;

  rec = (char *)&g_weapon_6a29b8 + slot->player * 0x1c;
  *(int *)(rec + 8) = 0;
  clump_set_flag_on_atomics(*(RpClump **)(rec + 0x14), 0x0);
  RpWorldRemoveClump((RpWorld *)course_get_rpworld_00467e90(),*(RpClump **)(rec + 0x14));
  return;
}


