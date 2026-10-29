// cluster-entries: endpoint_panel_attach_clump
/* Installed cluster endpoint_panel_attach_clump (tools/cluster_install.py from output/cluster_pilot11/endpoint_panel_attach_clump.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int atomic_geometry_user_data_int(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/* endpoint_panel_attach_clump */

/* "stores clump/frame, clears and fills the part table" */

/* Store clump and its frame in panel (+0x100/+0x104), clear the 0x80-byte part table at panel+0x80, then file each of the clump's 24 atomics into the slot returned by atomic_geometry_user_data_int (inferred part index). */
void endpoint_panel_attach_clump_n(int clump, int panel)

{
  undefined4 atomic;
  int part_slot;
  int atomic_index;
  undefined4 atomics [24];
  
  clump_get_atomics(clump,atomics);
  *(int *)(panel + 0x100) = clump;
  *(undefined4 *)(panel + 0x104) = *(undefined4 *)(clump + 4);
  mem_zero(panel + 0x80,0x80);
  atomic_index = 0;
  do {
    atomic = atomics[atomic_index];
    part_slot = atomic_geometry_user_data_int(atomic,0,0);
    atomic_index = atomic_index + 1;
    *(undefined4 *)(panel + 0x80 + part_slot * 4) = atomic;
  } while (atomic_index < 0x18);
  return;
}

/* entry thunk at the original address of endpoint_panel_attach_clump: register inputs (eax,ebx) -> natural C endpoint_panel_attach_clump_n; ret 0 */
__declspec(naked) void endpoint_panel_attach_clump(void)
{
    __asm {
        push ebx
        push eax
        call endpoint_panel_attach_clump_n
        add esp, 8
        ret
    }
}
