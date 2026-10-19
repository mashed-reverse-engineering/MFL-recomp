#include "mfl_tier2.h"
/*00658df0*/ extern undefined g_score_panel_658df0;
static int score_panel_entry_render_r(int reg_eax);
extern void score_panel_entry_render(void);
extern char g_score_panel_658fc0[];

#define ENTRY_SIZE (0x74)
#define ENTRY_ENABLED_OFFSET (0x6c)

__declspec(naked) int score_panel_entry_render_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call score_panel_entry_render
        ret
    }
}

/* Draws the score panel: walks the entry table from g_score_panel_658df0 up to
   g_score_panel_658fc0 (ENTRY_SIZE bytes per entry) and renders every entry whose
   enabled flag (ENTRY_ENABLED_OFFSET) is non-zero. Takes no parameters, returns nothing.
   (inferred) */

/* score_panel_render */

/* "entries that are enabled" */

void score_panel_render(void)

{
  undefined *entry;
  
  /* visit each entry in the table; stop once past the end of the array */
  entry = &g_score_panel_658df0;
  do {
    if (*(int *)(entry + ENTRY_ENABLED_OFFSET) != 0) {
      score_panel_entry_render_r(entry);
    }
    entry = entry + ENTRY_SIZE;
  } while ((LONG_PTR)entry < ((LONG_PTR)&g_score_panel_658fc0));
  return;
}


