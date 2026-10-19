// cluster-entries: team_panel_build_team_mask
/* Installed cluster team_panel_build_team_mask (tools/cluster_install.py from output/cluster_pilot11/team_panel_build_team_mask.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/* team_panel_build_team_mask */

/* WARNING: Unknown calling convention */
/* "mask 0x800000<<(n-1) and per slot team bits" */

void team_panel_build_team_mask_n(int in_eax)

{
  int iVar1;
  byte bVar2;
  int iVar3;
  char cVar4;
  
  iVar1 = *(int *)(in_eax + 0xc);
  *(int *)(in_eax + 0x168) = 0x800000 << ((char)iVar1 - 1U & 0x1f);
  iVar3 = 0;
  if (0 < iVar1) {
    cVar4 = '\0';
    do {
      bVar2 = (char)g_player_slots[*(int *)(in_eax + iVar3 * 4)].team + cVar4;
      cVar4 = cVar4 + '\x06';
      iVar3 = iVar3 + 1;
      *(uint *)(in_eax + 0x168) = *(uint *)(in_eax + 0x168) | 1 << (bVar2 & 0x1f);
    } while (iVar3 < iVar1);
  }
  return;
}

/* entry thunk at the original address of team_panel_build_team_mask: register inputs (eax) -> natural C team_panel_build_team_mask_n; ret 0 */
__declspec(naked) void team_panel_build_team_mask(void)
{
    __asm {
        push eax
        push eax
        call team_panel_build_team_mask_n
        add esp, 4
        pop eax
        ret
    }
}
