// cluster-entries: hudpanel_slot_attach_clump
/* Installed cluster hudpanel_slot_attach_clump (tools/cluster_install.py from output/cluster_pilot10/hudpanel_slot_attach_clump.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int atomic_geometry_user_data_int(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c53e0*/ extern void FUN_004c53e0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/* hudpanel_slot_attach_clump */

/* "stores clump and frame, clears the part table" */

void hudpanel_slot_attach_clump_n(int clump, int slot, undefined4 extra)

{
  int atomic;
  int part_index;
  int i;
  int atomics [6];
  
  clump_get_atomics(clump,atomics);
  *(undefined4 *)(slot + 0x48) = extra;
  *(int *)(slot + 0x40) = clump;
  *(undefined4 *)(slot + 0x44) = *(undefined4 *)(clump + 4);
  mem_zero(slot + 0x20,0x20);
  i = 0;
  do {
    atomic = atomics[i];
    part_index = atomic_geometry_user_data_int(atomic,0,0);
    *(int *)(slot + 0x20 + part_index * 4) = atomic;
    FUN_004c53e0(*(undefined4 *)(atomic + 0x18),0x40,1);
    i = i + 1;
  } while (i < 6);
  return;
}

/* entry thunk at the original address of hudpanel_slot_attach_clump: register inputs (eax,ebx,s0) -> natural C hudpanel_slot_attach_clump_n; ret 0 */
__declspec(naked) void hudpanel_slot_attach_clump(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ebx
        push eax
        call hudpanel_slot_attach_clump_n
        add esp, 12
        ret
    }
}
