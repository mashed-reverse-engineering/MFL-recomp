// cluster-entries: team_panel_load_origin
/* Installed cluster team_panel_load_origin (tools/cluster_install.py from output/cluster_pilot10/team_panel_load_origin.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00603f1c*/ extern undefined4 g_team_panel_origin_table;
/*00603f20*/ extern undefined4 g_team_panel_origin_table_y;
/*00603f24*/ extern undefined g_team_panel_origin_table_z;
/* team_panel_load_origin */

/* (was named team_panel_load_team_colour: "copies team colour") copies the 12-byte row of g_team_panel_origin_table for panel->team; the 3 floats are the panel origin (translation applied in team_panel_entry_update), not a colour */

void team_panel_load_origin_n(TeamPanelEntry *panel)

{
  int iVar1;
  
  iVar1 = panel->team;
  *(undefined4 *)&panel->origin.x = (&g_team_panel_origin_table)[iVar1 * 3];
  *(undefined4 *)&panel->origin.y = (&g_team_panel_origin_table_y)[iVar1 * 3];
  *(undefined4 *)&panel->origin.z = *(undefined4 *)((char *)&g_team_panel_origin_table_z + iVar1 * 0xc);
  return;
}

/* entry thunk at the original address of team_panel_load_origin: register inputs (ecx) -> natural C team_panel_load_origin_n; ret 0 */
__declspec(naked) void team_panel_load_origin(void)
{
    __asm {
        push ecx
        call team_panel_load_origin_n
        add esp, 4
        ret
    }
}
