#include "mfl_tier2.h"
/*00465410*/ extern int weapon_effect_slot_activate();
/*00467e90*/ extern void * course_get_rpworld_00467e90();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*004f4570*/ extern struct RpWorld * RpWorldAddClump(struct RpWorld *p0, struct RpClump *p1);
/*006a60d0*/ extern struct MachinegunState MachinegunState_ARRAY_006a60d0[4];
#define MACHINEGUN_START_AMMO (50)
/* weapon_machinegun_equip */
/* type 0x09; returns &machinegun_state[player] (0x6A60D0 stride 0x48) */
/* Equips the machinegun for the player who owns weapon_slot (inferred from the name and the body).
 * weapon_slot: the WeaponSlot that picked the gun up; its player index selects the MachinegunState entry.
 * Records the slot, adds the gun clump (GatlingGun.dff) to the course world, makes it visible, and resets
 * the state to a full magazine (MACHINEGUN_START_AMMO). Returns the player's MachinegunState. */
MachinegunState * weapon_machinegun_equip(WeaponSlot *weapon_slot)
{
  weapon_effect_slot_activate(weapon_slot->player);
  MachinegunState_ARRAY_006a60d0[weapon_slot->player].slot = (char *)weapon_slot;
  /* add the gun clump to the current course's world */
  RpWorldAddClump((RpWorld *)course_get_rpworld_00467e90(),(RpClump *)MachinegunState_ARRAY_006a60d0[weapon_slot->player].clump);
  /* flag 1 on the clump's atomics (inferred: makes the gun render) */
  clump_set_flag_on_atomics((RpClump *)MachinegunState_ARRAY_006a60d0[weapon_slot->player].clump, 0x1);
  /* reset the per-player state for a fresh gun */
  MachinegunState_ARRAY_006a60d0[weapon_slot->player].active = 1;
  MachinegunState_ARRAY_006a60d0[weapon_slot->player].fired = 0;
  MachinegunState_ARRAY_006a60d0[weapon_slot->player].ammo = MACHINEGUN_START_AMMO;
  MachinegunState_ARRAY_006a60d0[weapon_slot->player].spin = 0.0;
  /* unknown fields (field_0x1c/0x20/0x24/0x28) cleared to 0 */
  *(undefined4 *)&MachinegunState_ARRAY_006a60d0[weapon_slot->player].field_0x1c = 0;
  *(undefined4 *)&MachinegunState_ARRAY_006a60d0[weapon_slot->player].field_0x28 = 0;
  *(undefined4 *)&MachinegunState_ARRAY_006a60d0[weapon_slot->player].field_0x24 = 0;
  *(undefined4 *)&MachinegunState_ARRAY_006a60d0[weapon_slot->player].field_0x20 = 0;
  return MachinegunState_ARRAY_006a60d0 + weapon_slot->player;
}
