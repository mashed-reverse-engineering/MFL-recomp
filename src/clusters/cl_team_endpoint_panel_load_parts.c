// cluster-entries: team_endpoint_panel_load_parts
/* Installed cluster team_endpoint_panel_load_parts (tools/cluster_install.py from output/cluster_pilot10/team_endpoint_panel_load_parts.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c5380*/ extern void FUN_004c5380(undefined4 p0, undefined4 p1);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/* team_endpoint_panel_load_parts */

/* "loads grey 0x32 parts into the endpoint panel" */

void team_endpoint_panel_load_parts_n(int clump, int panel)

{
  undefined4 atomic;
  int part_slot;
  int i;
  struct {
    undefined1 local_8c;
    undefined1 local_8b;
    undefined1 local_8a;
    undefined1 local_89;
    undefined4 local_88 [34];
  } frame;
  
  frame.local_8c = 0x32;
  frame.local_8b = 0x32;
  frame.local_8a = 0x32;
  frame.local_89 = 0xff;
  clump_get_atomics(clump,frame.local_88);
  *(int *)(panel + 0x150) = clump;
  *(undefined4 *)(panel + 0x154) = *(undefined4 *)(clump + 4);
  mem_zero(panel + 0xb0,0xa0);
  i = 0;
  do {
    atomic = frame.local_88[i];
    part_slot = FUN_004c52b0(atomic,0,0);
    i = i + 1;
    *(undefined4 *)(panel + 0xb0 + part_slot * 4) = atomic;
  } while (i < 0x22);
  FUN_004c5380(*(undefined4 *)(panel + 0xb8),&frame.local_8c);
  return;
}

/* entry thunk at the original address of team_endpoint_panel_load_parts: register inputs (eax,ebx) -> natural C team_endpoint_panel_load_parts_n; ret 0 */
__declspec(naked) void team_endpoint_panel_load_parts(void)
{
    __asm {
        push ebx
        push eax
        call team_endpoint_panel_load_parts_n
        add esp, 8
        ret
    }
}
