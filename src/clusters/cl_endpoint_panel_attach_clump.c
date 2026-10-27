// cluster-entries: endpoint_panel_attach_clump
/* Installed cluster endpoint_panel_attach_clump (tools/cluster_install.py from output/cluster_pilot11/endpoint_panel_attach_clump.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/* endpoint_panel_attach_clump */

/* "stores clump/frame, clears and fills the part table" */

void endpoint_panel_attach_clump_n(int in_eax, int in_ebx)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined4 local_60 [24];
  
  clump_get_atomics(in_eax,local_60);
  *(int *)(in_ebx + 0x100) = in_eax;
  *(undefined4 *)(in_ebx + 0x104) = *(undefined4 *)(in_eax + 4);
  mem_zero(in_ebx + 0x80,0x80);
  iVar3 = 0;
  do {
    uVar1 = local_60[iVar3];
    iVar2 = FUN_004c52b0(uVar1,0,0);
    iVar3 = iVar3 + 1;
    *(undefined4 *)(in_ebx + 0x80 + iVar2 * 4) = uVar1;
  } while (iVar3 < 0x18);
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
