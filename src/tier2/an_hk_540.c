#include "mfl_tier2.h"
/*00465390*/ extern int weapon_effect_slot_clear();
/*00467e90*/ extern void * course_get_rpworld_00467e90();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*00484680*/ extern int object_set_flag_4();
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*006a5ff4*/ extern char *g_weapon_rocket_6a5ff4;
/*006a6008*/ extern char *g_weapon_6a6008;
/*006a6010*/ extern char *g_weapon_rocket_6a6010;

#define PLAYER_CLUMP_SLOT_WORDS (0xb)
#define PLAYER_ROCKET_STATE_SIZE (0x2c)

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
  char *rec;

  player_index = weapon_slot->player;
  rec = (char *)&g_rocket_state + player_index * PLAYER_ROCKET_STATE_SIZE;
  weapon_effect_slot_clear(player_index);
  RpWorldRemoveClump((RpWorld *)course_get_rpworld_00467e90(),*(RpClump **)(rec + 0x18));
  clump_set_flag_on_atomics(*(RpClump **)(rec + 0x18), 0x0);
  object_set_flag_4(*(undefined4 *)(rec + 0x20),0);
  *(undefined4 *)(rec + 4) = 0;
  return;
}


