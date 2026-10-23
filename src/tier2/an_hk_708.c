#include "mfl_tier2.h"
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*0042f860*/ extern int __cdecl texdict_load_named_b(char *p0, undefined4 p1, undefined4 p2);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*00658da8*/ extern undefined4 g_score_panel_658da8;
/*00658df0*/ extern undefined g_score_panel_658df0[0x1000];
static int score_panel_attach_clump_r(int reg_eax, void *reg_ebx);
extern void score_panel_attach_clump(void);
extern char g_score_panel_658fc0[];

#define PANEL_ENTRY_SIZE (0x74)
#define PANEL_ENTRY_INDEX_OFFSET (100)

__declspec(naked) int score_panel_attach_clump_r(int reg_eax, void *reg_ebx)
{
    __asm {
        push ebx
        mov eax, dword ptr [esp + 8]
        mov ebx, dword ptr [esp + 12]
        call score_panel_attach_clump
        pop ebx
        ret
    }
}

/* score_panel_load */

/*
 * Loads the score panel assets: the "panel.txd" texture dictionary (kept in
 * g_score_panel_658da8), then the clumps "panel0".."panel3". Each clump is
 * attached to the next PANEL_ENTRY_SIZE-byte entry of g_score_panel_658df0,
 * and the panel index is stored at PANEL_ENTRY_INDEX_OFFSET in that entry.
 * Returns nothing.
 * (was: "'panel.txd' + panel0..2 clumps", stale: four clumps are loaded)
 */

void score_panel_load(void)

{
  undefined4 clump;
  undefined *entry;
  int panel_index;
  char *panel_names [4];

  panel_names[0] = "panel0";
  panel_names[1] = "panel1";
  panel_names[2] = "panel2";
  panel_names[3] = "panel3";
  /* load the texture dictionary and make it the current one */
  g_score_panel_658da8 = texdict_load_named_b("panel.txd", 0x0, 0x0);
  /* (inferred) reset the current dictionary to none (NULL) */
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  panel_index = 0;
  entry = g_score_panel_658df0;
  /* one iteration per panel clump; the loop stops at the end of the
     entry array g_score_panel_658fc0, which is exactly 4 entries after
     g_score_panel_658df0 (inferred from the bound) */
  do {
    clump = clump_load_by_name(panel_names[panel_index], 0x0, 0x0);
    score_panel_attach_clump_r(clump,(void *)entry);
    *(int *)(entry + PANEL_ENTRY_INDEX_OFFSET) = panel_index;
    entry = entry + PANEL_ENTRY_SIZE;
    panel_index = panel_index + 1;
  } while ((char *)entry < ((char *)&g_score_panel_658fc0));
  return;
}


