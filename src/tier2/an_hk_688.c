#include "mfl_tier2.h"
/*00658df0*/ extern undefined g_score_panel_658df0;
static int score_panel_entry_reset_r(void *reg_ecx);
extern void score_panel_entry_reset(void);
extern char g_score_panel_658fc0[];

/* byte size of one score panel entry (inferred from the loop stride) */
#define SCORE_PANEL_ENTRY_SIZE (0x74)

__declspec(naked) int score_panel_entry_reset_r(void *reg_ecx)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call score_panel_entry_reset
        ret
    }
}

/* score_panel_reset_entries */

/* "score_panel_entry_reset for every entry" */

/* Resets every score panel entry. Walks the entry table from g_score_panel_658df0
 * up to g_score_panel_658fc0 in SCORE_PANEL_ENTRY_SIZE steps and calls
 * score_panel_entry_reset_r (entry pointer in ecx) on each one.
 * No parameters, no return value. */
void score_panel_reset_entries(void)

{
  char *entry_addr;
  undefined *entry;

  entry = &g_score_panel_658df0;
  /* loop over the table, stopping at the end address g_score_panel_658fc0 */
  do {
    entry_addr = (char *)(entry);
    score_panel_entry_reset_r((void *)entry);
    entry = (undefined *)(entry_addr + SCORE_PANEL_ENTRY_SIZE);
  } while ((char *)entry < ((char *)&g_score_panel_658fc0));
  return;
}


