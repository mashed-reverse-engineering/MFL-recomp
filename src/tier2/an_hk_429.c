#include "mfl_tier2.h"
/*00658fd8*/ extern undefined g_team_658fd8;
static int team_panel_entry_render_r(int reg_eax);
extern void team_panel_entry_render(void);
extern char g_team_6592b0[];

#define TEAM_ENTRY_STRIDE (0x16c)

__declspec(naked) int team_panel_entry_render_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call team_panel_entry_render
        ret
    }
}

/* team_panel_render */

/*
 * Renders the team panel: calls team_panel_entry_render once for each
 * team entry, walking the entry table from g_team_658fd8 up to
 * g_team_6592b0 in steps of TEAM_ENTRY_STRIDE bytes (inferred: the
 * table is laid out between those two globals). Takes no parameters
 * and returns nothing.
 */

void team_panel_render(void)

{
  undefined *team_entry;

  team_entry = &g_team_658fd8;
  /* Loop over the team entry table; the body runs at least once (do/while). */
  do {
    team_panel_entry_render_r(team_entry);
    team_entry = team_entry + TEAM_ENTRY_STRIDE;
  } while ((LONG_PTR)team_entry < ((LONG_PTR)&g_team_6592b0));
  return;
}


