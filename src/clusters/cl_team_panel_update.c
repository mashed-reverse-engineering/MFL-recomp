// cluster-entries: team_panel_update
/* Installed cluster team_panel_update (tools/cluster_install.py from output/cluster_pilot_zx/team_panel_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*0040ced0*/ extern int players_reached_target();
/*00658fd8*/ extern undefined g_team_658fd8;
/*006592b0*/ extern undefined g_team_6592b0;
/*006592c0*/ extern undefined4 g_hudpanel_6592c0;
#define TEAM_ENTRY_SIZE (0x16c) /* bytes per team entry in g_team_658fd8 (inferred) */

/* team_panel_update
 * Per-frame update of the team panels: asks players_reached_target() to fill
 * g_team_6592b0, then runs team_panel_entry_update() on each team entry in
 * g_team_658fd8 (TEAM_ENTRY_SIZE bytes apart, up to g_team_6592b0).
 * Also bumps the counter g_hudpanel_6592c0 (meaning inferred).
 * Takes no parameters, returns nothing.
 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "players_reached_target then team_panel_entry_update" */

static int team_panel_entry_update_r(int p0);
extern void team_panel_entry_update(void);
void team_panel_update(void)

{
  undefined *team_entry;

  /* Refresh the reached-target data before any entry is updated. */
  players_reached_target(&g_team_6592b0);
  team_entry = &g_team_658fd8;
  /* Walk the team entries from the first one up to the end of the table. */
  do {
    team_panel_entry_update_r(team_entry);
    team_entry = team_entry + TEAM_ENTRY_SIZE;
  } while ((LONG_PTR)team_entry < ((LONG_PTR)&g_team_6592b0));
  g_hudpanel_6592c0 = g_hudpanel_6592c0 + 1;
  return;
}

/* team_panel_entry_update_r: C -> original team_panel_entry_update (registers esi) */
__declspec(naked) int team_panel_entry_update_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call team_panel_entry_update
        pop esi
        ret
    }
}
