// cluster-entries: score_panel_attach_clump
/* Installed cluster score_panel_attach_clump (tools/cluster_install.py from output/cluster_pilot10/score_panel_attach_clump.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/* score_panel_attach_clump */

/* WARNING: Unknown calling convention */
/* "stores clump/frame, clears and fills the part table" */

void score_panel_attach_clump_n(int in_eax, int in_ebx)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined4 local_44 [17];
  
  clump_get_atomics(in_eax,local_44);
  *(int *)(in_ebx + 0x5c) = in_eax;
  *(undefined4 *)(in_ebx + 0x60) = *(undefined4 *)(in_eax + 4);
  mem_zero(in_ebx,0x50);
  iVar3 = 0;
  do {
    uVar1 = local_44[iVar3];
    iVar2 = FUN_004c52b0(uVar1,0,0);
    iVar3 = iVar3 + 1;
    *(undefined4 *)(in_ebx + iVar2 * 4) = uVar1;
  } while (iVar3 < 0x11);
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
