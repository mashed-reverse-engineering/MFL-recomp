// cluster-entries: hudpanel_slot_attach_clump
/* Installed cluster hudpanel_slot_attach_clump (tools/cluster_install.py from output/cluster_pilot10/hudpanel_slot_attach_clump.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c53e0*/ extern void FUN_004c53e0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/* hudpanel_slot_attach_clump */

/* WARNING: Unknown calling convention */
/* "stores clump and frame, clears the part table" */

void hudpanel_slot_attach_clump_n(int in_eax, int in_ebx, undefined4 param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int local_18 [6];
  
  clump_get_atomics(in_eax,local_18);
  *(undefined4 *)(in_ebx + 0x48) = param_3;
  *(int *)(in_ebx + 0x40) = in_eax;
  *(undefined4 *)(in_ebx + 0x44) = *(undefined4 *)(in_eax + 4);
  mem_zero(in_ebx + 0x20,0x20);
  iVar3 = 0;
  do {
    iVar1 = local_18[iVar3];
    iVar2 = FUN_004c52b0(iVar1,0,0);
    *(int *)(in_ebx + 0x20 + iVar2 * 4) = iVar1;
    FUN_004c53e0(*(undefined4 *)(iVar1 + 0x18),0x40,1);
    iVar3 = iVar3 + 1;
  } while (iVar3 < 6);
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
