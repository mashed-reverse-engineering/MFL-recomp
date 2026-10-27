#include "mfl_tier2.h"
/*00603f1c*/ extern undefined4 g_race_start_603f1c;
/*00658fd8*/ extern undefined g_team_658fd8;
/*006592c0*/ extern undefined4 g_hudpanel_6592c0;
static int team_panel_load_team_colour_r(int reg_ecx);
extern void team_panel_load_team_colour(void);
extern char g_team_6592b0[];

#define RACE_START_COLOUR_FLOATS (6)
#define TEAM_ENTRY_STRIDE (0x16c)
__declspec(naked) int team_panel_load_team_colour_r(int reg_ecx)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call team_panel_load_team_colour
        ret
    }
}

/* team_colour_table_init_2 */

/* "fills 6 floats (two team colours) at g_race_start_603f1c" */

/* Sets up the race-start team colours and team panel colours.
 * Copies a fixed table of 6 floats (two RGB colours, inferred) into g_race_start_603f1c,
 * loads the colour for each team entry from g_team_658fd8 up to g_team_6592b0 (inferred),
 * then sets g_hudpanel_6592c0 to 0. Takes no parameters, returns nothing. */
void team_colour_table_init_2(void)

{
  int words_left;
  undefined4 *src;
  undefined *team_entry;
  undefined4 *dst;
  undefined4 local_18 [RACE_START_COLOUR_FLOATS];
  
  /* float table: (0.45, 0.33, 1.0) and (-0.45, 0.33, 1.0) (inferred RGB, team 1 and 2) */
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


