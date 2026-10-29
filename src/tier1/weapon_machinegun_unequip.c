#include "mfl_tier2.h"
/*00465390*/ extern int weapon_effect_slot_clear();
/*00467e90*/ extern void * course_get_rpworld_00467e90();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*006a60d0*/ extern struct MachinegunState MachinegunState_ARRAY_006a60d0[4];
/* Removes the player's machinegun when it is unequipped (inferred from the name and the calls). */
/*
 * weapon_machinegun_unequip: removes the player's machinegun when it is unequipped (inferred from the name and the calls).
 *   weapon_slot: WeaponSlot whose player index selects the MachinegunState entry.
 * Clears the effect slot, marks the state as empty and inactive, removes the gun's clump (GatlingGun.dff)
 * from the current course's RpWorld, then clears its flag on the atomics (inferred: flag value 0 = off).
 */
void weapon_machinegun_unequip(WeaponSlot *weapon_slot)
{
  uint player_index;
  MachinegunState *rec;

  player_index = weapon_slot->player;
  rec = &MachinegunState_ARRAY_006a60d0[player_index];
  weapon_effect_slot_clear(player_index);
  rec->slot = 0;
  rec->active = 0;
  RpWorldRemoveClump((RpWorld *)course_get_rpworld_00467e90(),(RpClump *)rec->clump);
  clump_set_flag_on_atomics((RpClump *)rec->clump, 0x0);
  return;
}
