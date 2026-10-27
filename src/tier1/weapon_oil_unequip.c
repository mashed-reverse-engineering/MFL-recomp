#include "mfl_tier2.h"
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/* layout of *rec in weapon_oil_unequip, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct weapon_oil_unequip_rec_layout {
    undefined4 f_0x0;
    undefined4 f_0x4;
    char pad_0x8[4];
    union { RpClump * as_RpClump; undefined4 as_undefined4; } f_0xc;
} weapon_oil_unequip_rec_layout;

/*006a7c78*/ extern undefined g_weapon_6a7c78;
#define PLAYER_RECORD_SIZE (0x10) /* bytes per player in the g_weapon_6a7c78 table (inferred) */
/*
 * Unequips the oil weapon for one player: clears that player's weapon record,
 * clears the flag on the clump held in the record, and removes the clump from
 * its world if it is in one. (inferred)
 * weapon_slot: the player's weapon slot; only weapon_slot->player is read.
 * Returns nothing.
 */
void weapon_oil_unequip(WeaponSlot *weapon_slot)
{
  weapon_oil_unequip_rec_layout *rec;
  RpWorld *world;
  RpClump *clump;

  /* the player's per-player record: +0 and +4 are zeroed, +0xc holds the clump pointer (inferred) */
  rec = (weapon_oil_unequip_rec_layout *)((char *)&g_weapon_6a7c78 + weapon_slot->player * PLAYER_RECORD_SIZE);
  clump = rec->f_0xc.as_RpClump;
  rec->f_0x4 = 0;
  rec->f_0x0 = 0;
  clump_set_flag_on_atomics(clump, 0x0);
  /* remove the clump from its world, if it has one */
  world = (RpWorld *)clump_get_world(rec->f_0xc.as_undefined4);
  if (world != (RpWorld *)0x0) {
    RpWorldRemoveClump(world,rec->f_0xc.as_RpClump);
  }
  return;
}
