// cluster-entries: score_panel_reset_entries
/* Installed cluster score_panel_reset_entries (tools/cluster_install.py from output/cluster_pilot_zx/score_panel_reset_entries.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*00658df0*/ extern undefined g_score_panel_658df0;
extern char g_score_panel_658fc0[];

/* byte size of one score panel entry (inferred from the loop stride) */
#define SCORE_PANEL_ENTRY_SIZE (0x74)


/* score_panel_reset_entries */

/* "score_panel_entry_reset for every entry" */

/* Resets every score panel entry. Walks the entry table from g_score_panel_658df0
 * up to g_score_panel_658fc0 in SCORE_PANEL_ENTRY_SIZE steps and calls
 * score_panel_entry_reset (entry pointer in ecx) on each one.
 * No parameters, no return value. */
static int score_panel_entry_reset_r(void * p0);
extern void score_panel_entry_reset(void);
void score_panel_reset_entries(void)

{
  int entry_addr;
  undefined *entry;

  entry = &g_score_panel_658df0;
  /* loop over the table, stopping at the end address g_score_panel_658fc0 */
  do {
    entry_addr = (int)(entry);
    score_panel_entry_reset_r((void *)entry);
    entry = (undefined *)(entry_addr + SCORE_PANEL_ENTRY_SIZE);
  } while ((int)entry < ((int)&g_score_panel_658fc0));
  return;
}

/* score_panel_entry_reset_r: C -> original score_panel_entry_reset (registers ecx) */
__declspec(naked) int score_panel_entry_reset_r(void * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call score_panel_entry_reset
        ret
    }
}
