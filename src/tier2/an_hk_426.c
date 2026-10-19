#include "mfl_tier2.h"
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00658da8*/ extern undefined4 g_score_panel_658da8;
/*00658df0*/ extern undefined g_score_panel_658df0;
static int score_panel_entry_destroy_r(int reg_eax);
extern void score_panel_entry_destroy(void);
extern char g_score_panel_658fc0[];
#define SCORE_PANEL_ENTRY_SIZE (0x74) /* bytes per score panel entry (inferred) */
__declspec(naked) int score_panel_entry_destroy_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call score_panel_entry_destroy
        ret
    }
}

/* score_panel_destroy */

/* Destroys the score panel: runs score_panel_entry_destroy on every entry in
   the entry array (g_score_panel_658df0 up to g_score_panel_658fc0, stride
   SCORE_PANEL_ENTRY_SIZE), then destroys the score panel texture dictionary
   g_score_panel_658da8. Takes no parameters and returns nothing. (inferred) */

void score_panel_destroy(void)

{
  undefined *entry;

  entry = &g_score_panel_658df0;
  /* walk the entry array, one entry per iteration */
  do {
    score_panel_entry_destroy_r(entry);
    entry = entry + SCORE_PANEL_ENTRY_SIZE;
  } while ((LONG_PTR)entry < ((LONG_PTR)&g_score_panel_658fc0));
  RwTexDictionaryDestroy_004d5ac0(g_score_panel_658da8);
  return;
}


