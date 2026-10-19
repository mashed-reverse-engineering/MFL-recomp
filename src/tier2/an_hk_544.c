#include "mfl_tier2.h"
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/*006a7c78*/ extern undefined g_weapon_6a7c78;
/*006a7c7c*/ extern undefined4 g_weapon_6a7c7c;
/*006a7c84*/ extern undefined4 g_weapon_6a7c84;
#define PLAYER_RECORD_SIZE (0x10) /* bytes per player in the g_weapon_6a7c78 table (inferred) */
#define PLAYER_RECORD_DWORDS (4) /* PLAYER_RECORD_SIZE / sizeof(undefined4) */
/*
 * Unequips the oil weapon for one player: clears that player's weapon record,
 * clears the flag on the clump held in the record, and removes the clump from
 * its world if it is in one. (inferred)
 * weapon_slot: the player's weapon slot; only weapon_slot->player is read.
 * Returns nothing.
 */

void weapon_oil_unequip(WeaponSlot *weapon_slot)

{
  char *rec;
  RpWorld *world;
  RpClump *clump;

  /* the player's per-player record: +0 and +4 are zeroed, +0xc holds the clump pointer (inferred) */
  rec = (char *)&g_weapon_6a7c78 + weapon_slot->player * PLAYER_RECORD_SIZE;
  clump = *(RpClump **)(rec + 0xc);
  *(undefined4 *)(rec + 4) = 0;
  *(undefined4 *)rec = 0;
  clump_set_flag_on_atomics(clump, 0x0);
  /* remove the clump from its world, if it has one */
  world = (RpWorld *)clump_get_world(*(undefined4 *)(rec + 0xc));
  if (world != (RpWorld *)0x0) {
    RpWorldRemoveClump(world,*(RpClump **)(rec + 0xc));
  }
  return;
}


