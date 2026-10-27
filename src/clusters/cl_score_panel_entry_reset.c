// cluster-entries: score_panel_entry_reset
/* Installed cluster score_panel_entry_reset (tools/cluster_install.py from output/cluster_pilot10/score_panel_entry_reset.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00603ed4*/ extern undefined g_score_panel_603ed4;
/*00603ed8*/ extern undefined g_score_panel_603ed8;
/*00603edc*/ extern undefined g_score_panel_603edc;
/* score_panel_entry_reset */

/* "entry scale / offsets from the table at 0x603ED4" */

void score_panel_entry_reset_n(int in_ecx)

{
  float *pfVar1;
  int iVar2;
  
  pfVar1 = (float *)(in_ecx + 0x50);
  iVar2 = *(int *)(in_ecx + 100) * 0xc;
  *pfVar1 = *(float *)((char *)&g_score_panel_603ed4 + iVar2);
  *(undefined4 *)(in_ecx + 0x54) = *(undefined4 *)((char *)&g_score_panel_603ed8 + iVar2);
  *(undefined4 *)(in_ecx + 0x58) = *(undefined4 *)((char *)&g_score_panel_603edc + iVar2);
  *(undefined4 *)(in_ecx + 0x70) = 0;
  *pfVar1 = *pfVar1 + *pfVar1;
  return;
}

/* entry thunk at the original address of score_panel_entry_reset: register inputs (ecx) -> natural C score_panel_entry_reset_n; ret 0 */
__declspec(naked) void score_panel_entry_reset(void)
{
    __asm {
        push ecx
        push ecx
        call score_panel_entry_reset_n
        add esp, 4
        pop ecx
        ret
    }
}
