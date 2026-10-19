#include "mfl_tier2.h"
/*0040ce30*/ extern int score_target_a();
/*0040fdb0*/ extern int get_player_count();
/*004b4ba7*/ extern int entry();
/*00659304*/ extern undefined4 g_kc_panel_659304;
/*00659338*/ extern undefined4 max_points_b;
/*00659340*/ extern undefined g_kc_659340;
static int kc_panel_entry_init_r(int reg_esi);
extern void kc_panel_entry_init(void);
extern char g_kc_659790[];
#define KC_PANEL_ENTRY_STRIDE (0x114)
__declspec(naked) int kc_panel_entry_init_r(int reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call kc_panel_entry_init
        pop esi
        ret
    }
}

/* kc_panel_init */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "player count, max_points_b = score_target_a, entry init" */

/*
 * Sets up the KC panel at startup (inferred: the panel is driven by the
 * entry table below). Stores the player count in g_kc_panel_659304 and the
 * score target in max_points_b, then calls kc_panel_entry_init_r once per
 * table entry. Takes no parameters, returns nothing.
 */
void kc_panel_init(void)

{
  undefined *entry_ptr;
  
  g_kc_panel_659304 = get_player_count();
  max_points_b = score_target_a();
  entry_ptr = &g_kc_659340;
  /* Walk the entry table from g_kc_659340 up to g_kc_659790, one entry per
     KC_PANEL_ENTRY_STRIDE bytes, initialising each entry. */
  do {
    kc_panel_entry_init_r(entry_ptr);
    entry_ptr = entry_ptr + KC_PANEL_ENTRY_STRIDE;
  } while ((LONG_PTR)entry_ptr < ((LONG_PTR)&g_kc_659790));
  return;
}


