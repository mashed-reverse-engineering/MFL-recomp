// cluster-entries: score_panel_attach_clump
/* Installed cluster score_panel_attach_clump (tools/cluster_install.py from output/cluster_pilot10/score_panel_attach_clump.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/* score_panel_attach_clump */

/* "stores clump/frame, clears and fills the part table" */

void score_panel_attach_clump_n(int clump, int panel)

{
  undefined4 atomic;
  int part_index;
  int i;
  undefined4 atomics [17];
  
  clump_get_atomics(clump,atomics);
  *(int *)(panel + 0x5c) = clump;
  *(undefined4 *)(panel + 0x60) = *(undefined4 *)(clump + 4);
  mem_zero(panel,0x50);
  i = 0;
  do {
    atomic = atomics[i];
    part_index = FUN_004c52b0(atomic,0,0);
    i = i + 1;
    *(undefined4 *)(panel + part_index * 4) = atomic;
  } while (i < 0x11);
  return;
}

/* entry thunk at the original address of score_panel_attach_clump: register inputs (eax,ebx) -> natural C score_panel_attach_clump_n; ret 0 */
__declspec(naked) void score_panel_attach_clump(void)
{
    __asm {
        push ebx
        push eax
        call score_panel_attach_clump_n
        add esp, 8
        ret
    }
}
