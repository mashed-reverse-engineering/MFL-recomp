#include "mfl_tier2.h"
/*004c4f90*/ extern int __cdecl FUN_004c4f90(int p0, undefined4 p1, undefined4 p2);
/*0054d6a0*/ extern void __cdecl FUN_0054d6a0(undefined4 p0);
/*00659f00*/ extern struct VehicleRecord g_vehicle_table[4];
/* vehicle_part_set_effect */
/* "(player, part, a, b): locks the part's geometry and applies FUN_004c4f90 (material effect,
   inferred)"
   Returns FUN_004c4f90's result (atomic_line_closest_hit: hit or 0); weapon_find_targets_in_cone 0x465875 tests it
   (VOID_w16: was void). */
int __cdecl vehicle_part_set_effect(int player,int part,undefined4 effect_a,undefined4 effect_b)
{
  int part_frame;

  part_frame = g_vehicle_table[player].part_frames[part];
  FUN_0054d6a0(*(undefined4 *)(part_frame + 0x18));
  return FUN_004c4f90(part_frame,effect_a,effect_b);
}
