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
void weapon_rocket_unequip(WeaponSlot *weapon_slot)

{
  uint player_index;
  RpWorld *world;
  RpClump *rocket_clump;
  undefined4 object_flags;

  player_index = weapon_slot->player;
  weapon_effect_slot_clear(player_index);

  /* Take the player's rocket clump out of the course world. */
  rocket_clump = (RpClump *)(&g_weapon_6a6008)[player_index * PLAYER_CLUMP_SLOT_WORDS];
  world = (RpWorld *)course_get_rpworld_00467e90();
  RpWorldRemoveClump(world,rocket_clump);

  /* Clear the flag on the clump's atomics and reset the rocket state (inferred). */
  rocket_clump = (RpClump *)(&g_weapon_6a6008)[player_index * PLAYER_CLUMP_SLOT_WORDS];
  object_flags = 0;
  clump_set_flag_on_atomics(rocket_clump, 0x0);
  object_set_flag_4(*(undefined4 *)((char *)&g_weapon_rocket_6a6010 + player_index * PLAYER_ROCKET_STATE_SIZE),0,rocket_clump,object_flags);
  *(undefined4 *)((char *)&g_weapon_rocket_6a5ff4 + player_index * PLAYER_ROCKET_STATE_SIZE) = 0;
  return;
}


