// cluster-entries: score_panel_init
/* Installed cluster score_panel_init (tools/cluster_install.py from output/cluster_pilot_zx/score_panel_init.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*0040ce30*/ extern int score_target_a();
/*0040fdb0*/ extern int get_player_count();
/*004b4ba7*/ extern int entry();
/*00658dbc*/ extern undefined4 g_score_panel_658dbc;
/*00658de4*/ extern undefined4 max_points_a;
/*00658df0*/ extern undefined g_score_panel_658df0;
extern char g_score_panel_658fc0[];
#define SCORE_PANEL_ENTRY_SIZE (0x74)

/* score_panel_init
 * Sets up the score panel at startup: stores the player count and the score
 * target (max_points_a), then runs score_panel_entry_init on every panel entry
 * in the table that starts at g_score_panel_658df0 and ends at g_score_panel_658fc0.
 * Takes no parameters and returns nothing. The entry table layout is (inferred).
 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "player count, max_points_a = score_target_a, score_panel_entry_init for every entry (0x658DF0,
   stride 0x74)" */

static int score_panel_entry_init_r(int p0);
extern void score_panel_entry_init(void);
void score_panel_init(void)

{
  undefined *score_entry;

  g_score_panel_658dbc = get_player_count();
  max_points_a = score_target_a();
  score_entry = &g_score_panel_658df0;
  /* walk the entry table, one SCORE_PANEL_ENTRY_SIZE block at a time, until the end address (inferred) */
  do {
    score_panel_entry_init_r(score_entry);
    score_entry = score_entry + SCORE_PANEL_ENTRY_SIZE;
  } while ((LONG_PTR)score_entry < ((LONG_PTR)&g_score_panel_658fc0));
  return;
}

/* score_panel_entry_init_r: C -> original score_panel_entry_init (registers esi) */
__declspec(naked) int score_panel_entry_init_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call score_panel_entry_init
        pop esi
        ret
    }
}
