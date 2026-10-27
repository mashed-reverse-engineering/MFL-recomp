#include "mfl_tier2.h"
/*00465390*/ extern int weapon_effect_slot_clear();
/*00467e90*/ extern void * course_get_rpworld_00467e90();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*00484680*/ extern int object_set_flag_4();
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
#define PLAYER_ROCKET_STATE_SIZE (0x2c)
/* layout of *rec in weapon_rocket_unequip, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct weapon_rocket_unequip_rec_layout {
    char pad_0x0[4];
    undefined4 f_0x4;
    char pad_0x8[16];
    RpClump * f_0x18;
    char pad_0x1c[4];
    undefined4 f_0x20;
} weapon_rocket_unequip_rec_layout;

/*
 * Unequips the rocket weapon held in weapon_slot (inferred): clears the weapon
 * effect slot, removes the player's rocket clump from the course world, clears
 * the clump's atomics flag, and zeroes the player's rocket state.
 * weapon_slot: slot whose ->player index selects the per-player entries.
 * Returns nothing.
 */
extern char g_rocket_state[];
void weapon_rocket_unequip(WeaponSlot *weapon_slot)
{
  uint player_index;
  weapon_rocket_unequip_rec_layout *rec;

  player_index = weapon_slot->player;
  rec = (weapon_rocket_unequip_rec_layout *)((char *)&g_rocket_state + player_index * PLAYER_ROCKET_STATE_SIZE);
  weapon_effect_slot_clear(player_index);
  RpWorldRemoveClump((RpWorld *)course_get_rpworld_00467e90(),rec->f_0x18);
  clump_set_flag_on_atomics(rec->f_0x18, 0x0);
  object_set_flag_4(rec->f_0x20,0);
  rec->f_0x4 = 0;
  return;
}
