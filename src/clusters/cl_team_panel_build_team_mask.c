// cluster-entries: team_panel_build_team_mask
/* Installed cluster team_panel_build_team_mask (tools/cluster_install.py from output/cluster_pilot11/team_panel_build_team_mask.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/* team_panel_build_team_mask */

/* "mask 0x800000<<(n-1) and per slot team bits" */

void team_panel_build_team_mask_n(TeamPanelEntry *panel)

{
  int slot_count;
  byte bit;
  int i;
  char shift;
  
  slot_count = panel->player_count;
  panel->flags = 0x800000 << ((char)slot_count - 1U & 0x1f);
  i = 0;
  if (0 < slot_count) {
    shift = '\0';
    do {
      bit = (char)g_player_slots[panel->player[i]].team + shift;
      shift = shift + '\x06';
      i = i + 1;
      panel->flags = panel->flags | 1 << (bit & 0x1f);
    } while (i < slot_count);
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
