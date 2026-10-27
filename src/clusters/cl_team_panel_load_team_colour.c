// cluster-entries: team_panel_load_team_colour
/* Installed cluster team_panel_load_team_colour (tools/cluster_install.py from output/cluster_pilot10/team_panel_load_team_colour.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00603f1c*/ extern undefined4 g_race_start_603f1c;
/*00603f20*/ extern undefined4 g_race_start_603f20;
/*00603f24*/ extern undefined g_race_start_603f24;
/* team_panel_load_team_colour */

/* "copies team colour (table 0x603F1C, 12 bytes per team) into the panel" */

void team_panel_load_team_colour_n(TeamPanelEntry *panel)

{
  int iVar1;
  
  iVar1 = panel->team;
  *(undefined4 *)&panel->origin.x = (&g_race_start_603f1c)[iVar1 * 3];
  *(undefined4 *)&panel->origin.y = (&g_race_start_603f20)[iVar1 * 3];
  *(undefined4 *)&panel->origin.z = *(undefined4 *)((char *)&g_race_start_603f24 + iVar1 * 0xc);
  return;
}

/* entry thunk at the original address of team_panel_load_team_colour: register inputs (ecx) -> natural C team_panel_load_team_colour_n; ret 0 */
__declspec(naked) void team_panel_load_team_colour(void)
{
    __asm {
        push ecx
        call team_panel_load_team_colour_n
        add esp, 4
        ret
    }
}
