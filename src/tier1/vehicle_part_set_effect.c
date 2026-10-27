#include "mfl_tier2.h"
/*004c4f90*/ extern void __cdecl FUN_004c4f90(int p0, undefined4 p1, undefined4 p2);
/*0054d6a0*/ extern void __cdecl FUN_0054d6a0(undefined4 p0);
/*00659f00*/ extern struct VehicleRecord g_vehicle_table[4];
/* vehicle_part_set_effect */
/* "(player, part, a, b): locks the part's geometry and applies FUN_004c4f90 (material effect,
   inferred)" */
void __cdecl vehicle_part_set_effect(int param_1,int param_2,undefined4 param_3,undefined4 param_4)
{
  int iVar1;
  
  iVar1 = g_vehicle_table[param_1].part_frames[param_2];
  FUN_0054d6a0(*(undefined4 *)(iVar1 + 0x18));
  FUN_004c4f90(iVar1,param_3,param_4);
  return;
}
