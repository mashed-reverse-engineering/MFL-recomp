// cluster-entries: team_panel_load_parts
/* Installed cluster team_panel_load_parts (tools/cluster_install.py from output/cluster_pilot11/team_panel_load_parts.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c5380*/ extern void FUN_004c5380(undefined4 p0, undefined4 p1);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/* team_panel_load_parts */

/* WARNING: Unknown calling convention */
/* "loads the 0x32 grey colour parts into the team panel" */

void team_panel_load_parts_n(int in_eax, int in_ebx)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  struct {
    undefined1 local_78;
    undefined1 local_77;
    undefined1 local_76;
    undefined1 local_75;
    undefined4 local_74 [29];
  } frame;
  
  frame.local_78 = 0x32;
  frame.local_77 = 0x32;
  frame.local_76 = 0x32;
  frame.local_75 = 0xff;
  clump_get_atomics(in_eax,frame.local_74);
  *(int *)(in_ebx + 0x15c) = in_eax;
  *(undefined4 *)(in_ebx + 0x160) = *(undefined4 *)(in_eax + 4);
  mem_zero(in_ebx + 0xb0,0xa0);
  iVar3 = 0;
  do {
    uVar1 = frame.local_74[iVar3];
    iVar2 = FUN_004c52b0(uVar1,0,0);
    iVar3 = iVar3 + 1;
    *(undefined4 *)(in_ebx + 0xb0 + iVar2 * 4) = uVar1;
  } while (iVar3 < 0x1d);
  FUN_004c5380(*(undefined4 *)(in_ebx + 0xb8),&frame.local_78);
  return;
}

/* entry thunk at the original address of team_panel_load_parts: register inputs (eax,ebx) -> natural C team_panel_load_parts_n; ret 0 */
__declspec(naked) void team_panel_load_parts(void)
{
    __asm {
        push ebx
        push eax
        call team_panel_load_parts_n
        add esp, 8
        ret
    }
}
