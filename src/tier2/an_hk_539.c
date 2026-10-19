#include "mfl_tier2.h"
/*00465410*/ extern int weapon_effect_slot_activate();
/*00467e90*/ extern void * course_get_rpworld_00467e90();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*004f4570*/ extern struct RpWorld * RpWorldAddClump(struct RpWorld *p0, struct RpClump *p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/*006a5ff0*/ extern char *g_rocket_state;
/*006a5ff4*/ extern char *g_weapon_rocket_6a5ff4;
/*006a5ff8*/ extern char *g_weapon_6a5ff8;
/*006a6008*/ extern undefined4 g_weapon_6a6008;
/*006a600c*/ extern char *g_weapon_rocket_6a600c;
/*006a6018*/ extern char *g_weapon_rocket_6a6018;

#define PLAYER_STATE_SIZE (0x2c)
#define PLAYER_CLUMP_DWORD_STRIDE (0xb)

/* weapon_rocket_equip */

/* type 0x0B; state 0x6A5FF0 stride 0x2C */

/* Equips the rocket weapon for the player that owns `slot`: activates the weapon
   effect slot, resets the player's rocket state (slot pointer and flags, inferred),
   and makes sure the player's clump is in the current course's RpWorld.
   Returns the player's rocket state block (inferred). */
undefined4 * weapon_rocket_equip(WeaponSlot *slot)

{
  uint player_index;
  undefined4 player_clump_raw;
  RpWorld *clump_world;
  RpWorld *course_world;
  int player_state_offset;
  RpClump *clump;

  player_index = slot->player;
  weapon_effect_slot_activate(player_index);
  player_state_offset = player_index * PLAYER_STATE_SIZE;
  *(undefined4 *)((char *)&g_weapon_rocket_6a5ff4 + player_state_offset) = 1;
  *(undefined4 *)((char *)&g_weapon_6a5ff8 + player_state_offset) = 1;
  player_clump_raw = (&g_weapon_6a6008)[player_index * PLAYER_CLUMP_DWORD_STRIDE];
  *(WeaponSlot **)((char *)&g_rocket_state + player_state_offset) = slot;
  *(undefined4 *)((char *)&g_weapon_rocket_6a6018 + player_state_offset) = 0;
  *(undefined4 *)((char *)&g_weapon_rocket_6a600c + player_state_offset) = 0;
  clump_world = (RpWorld *)clump_get_world(player_clump_raw);
  course_world = (RpWorld *)course_get_rpworld_00467e90();
  /* Clump is in a different world than the course: detach it from the old world
     (if any), then attach it to the course world. */
  if (clump_world != course_world) {
    if (clump_world != (RpWorld *)0x0) {
      RpWorldRemoveClump(clump_world,(RpClump *)(&g_weapon_6a6008)[player_index * PLAYER_CLUMP_DWORD_STRIDE]);
    }
    clump = (RpClump *)(&g_weapon_6a6008)[player_index * PLAYER_CLUMP_DWORD_STRIDE];
    clump_world = (RpWorld *)course_get_rpworld_00467e90();
    RpWorldAddClump(clump_world,clump);
  }
  /* Set the flag (0x1) on all atomics of the player's clump. */
  clump_set_flag_on_atomics((RpClump *)(&g_weapon_6a6008)[player_index * PLAYER_CLUMP_DWORD_STRIDE], 0x1);
  return (undefined4 *)((char *)&g_rocket_state + player_state_offset);
}


