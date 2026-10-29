// cluster-entries: team_colour_table_init_2
/* Installed cluster team_colour_table_init_2 (tools/cluster_install.py from output/cluster_pilot_zx/team_colour_table_init_2.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*00603f1c*/ extern undefined4 g_race_start_603f1c;
/*00658fd8*/ extern undefined g_team_658fd8;
/*006592c0*/ extern undefined4 g_hudpanel_6592c0;
extern char g_team_6592b0[];

#define RACE_START_COLOUR_FLOATS (6)
#define TEAM_ENTRY_STRIDE (0x16c)

/* team_colour_table_init_2 */

/* "fills 6 floats (two team colours) at g_race_start_603f1c" -- the values are positions, see below */

/* Sets up the two team panel origins (naming note: the function and table names say colour, the data is a position).
 * Copies a fixed table of 6 floats (two panel positions (+-0.45, 0.33, 1.0), x mirrored; not colours) into g_race_start_603f1c,
 * copies the row of each team panel entry (g_team_658fd8 up to g_team_6592b0) into its origin via team_panel_load_team_colour,
 * then sets g_hudpanel_6592c0 to 0. Takes no parameters, returns nothing. */
static int team_panel_load_team_colour_r(int p0);
extern void team_panel_load_team_colour(void);
void team_colour_table_init_2(void)

{
  int words_left;
  undefined4 *src;
  undefined *team_entry;
  undefined4 *dst;
  undefined4 local_18 [RACE_START_COLOUR_FLOATS];
  
  /* float table: (0.45, 0.33, 1.0) and (-0.45, 0.33, 1.0) (panel origin x, y, z of team panel 0 and 1, not RGB) */
  local_18[0] = 0x3ee66666;
  local_18[1] = 0x3ea8f5c3;
  local_18[2] = 0x3f800000;
  local_18[3] = 0xbee66666;
  local_18[4] = 0x3ea8f5c3;
  local_18[5] = 0x3f800000;
  src = local_18;
  dst = &g_race_start_603f1c;
  /* copy the float table into g_race_start_603f1c, one 32-bit word at a time */
  for (words_left = RACE_START_COLOUR_FLOATS; words_left != 0; words_left = words_left + -1) {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  }
  /* walk the team table, one TEAM_ENTRY_STRIDE-sized entry at a time, loading each team's colour */
  team_entry = &g_team_658fd8;
  do {
    team_panel_load_team_colour_r(team_entry);
    team_entry = team_entry + TEAM_ENTRY_STRIDE;
  } while ((LONG_PTR)team_entry < ((LONG_PTR)&g_team_6592b0));
  g_hudpanel_6592c0 = 0;
  return;
}

/* team_panel_load_team_colour_r: C -> original team_panel_load_team_colour (registers ecx) */
__declspec(naked) int team_panel_load_team_colour_r(int p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call team_panel_load_team_colour
        ret
    }
}
