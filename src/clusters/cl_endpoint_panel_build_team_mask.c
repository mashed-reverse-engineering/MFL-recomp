// cluster-entries: endpoint_panel_build_team_mask
/* Installed cluster endpoint_panel_build_team_mask (tools/cluster_install.py from output/cluster_pilot11/endpoint_panel_build_team_mask.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/* endpoint_panel_build_team_mask */

/* WARNING: Unknown calling convention */
/* "mask 0x800000<<(n-1) and per slot team bits" */

void endpoint_panel_build_team_mask_n(int panel)

{
  int slot_count;
  byte bit;
  int i;
  char shift;
  
  slot_count = *(int *)(panel + 0xc);
  *(int *)(panel + 0x15c) = 0x800000 << ((char)slot_count - 1U & 0x1f);
  i = 0;
  if (0 < slot_count) {
    shift = '\0';
    do {
      bit = (char)g_player_slots[*(int *)(panel + i * 4)].team + shift;
      shift = shift + '\x06';
      i = i + 1;
      *(uint *)(panel + 0x15c) = *(uint *)(panel + 0x15c) | 1 << (bit & 0x1f);
    } while (i < slot_count);
  }
  return;
}

/* entry thunk at the original address of endpoint_panel_build_team_mask: register inputs (eax) -> natural C endpoint_panel_build_team_mask_n; ret 0 */
__declspec(naked) void endpoint_panel_build_team_mask(void)
{
    __asm {
        push eax
        push eax
        call endpoint_panel_build_team_mask_n
        add esp, 4
        pop eax
        ret
    }
}
