#include "mfl_tier2.h"
/*00658fd8*/ extern undefined g_team_658fd8;
extern void team_panel_entry_destroy_n(TeamPanelEntry *panel);
extern char g_team_6592b0[];

#define TEAM_PANEL_ENTRY_SIZE (0x16c)


/* team_panel_destroy */

/*
 * Destroys every team panel entry in the table g_team_658fd8 .. g_team_6592b0
 * (end bound is the address of g_team_6592b0). Each entry is passed to
 * team_panel_entry_destroy_r, which calls team_panel_entry_destroy (inferred:
 * releases that entry's resources). No parameters, no return value.
 */

void team_panel_destroy(void)

{
  undefined *panel_entry;

  panel_entry = &g_team_658fd8;
  /* Loop: visit each entry from the start of the table until the end bound. */
  do {
    team_panel_entry_destroy_n((TeamPanelEntry *)panel_entry);
    panel_entry = panel_entry + TEAM_PANEL_ENTRY_SIZE;
  } while ((LONG_PTR)panel_entry < ((LONG_PTR)&g_team_6592b0));
  return;
}


