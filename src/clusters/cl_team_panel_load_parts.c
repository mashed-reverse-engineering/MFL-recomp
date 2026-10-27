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

void team_panel_load_parts_n(int clump, TeamPanelEntry *panel)

{
  undefined4 atomic;
  int part_slot;
  int i;
  struct {
    undefined1 grey_r;
    undefined1 grey_g;
    undefined1 grey_b;
    undefined1 grey_a;
    undefined4 atomics [29];
  } frame;
  
  frame.grey_r = 0x32;
  frame.grey_g = 0x32;
  frame.grey_b = 0x32;
  frame.grey_a = 0xff;
  clump_get_atomics(clump,frame.atomics);
  panel->clump = clump;
  panel->frame = *(undefined4 *)(clump + 4);
  mem_zero(panel->part,sizeof(panel->part));
  i = 0;
  do {
    atomic = frame.atomics[i];
    part_slot = FUN_004c52b0(atomic,0,0);
    i = i + 1;
    *(undefined4 *)&panel->part[part_slot] = atomic;
  } while (i < 0x1d);
  FUN_004c5380(*(undefined4 *)&panel->part[2],&frame.grey_r);
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
