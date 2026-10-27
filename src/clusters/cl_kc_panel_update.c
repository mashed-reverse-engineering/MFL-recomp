// cluster-entries: kc_panel_update
/* Installed cluster kc_panel_update (tools/cluster_install.py from output/cluster_pilot_zx/kc_panel_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*0040cad0*/ extern int players_get_points();
/*0040ced0*/ extern int players_reached_target();
/*0040cf10*/ extern int players_have_zero_points();
/*0040cf40*/ extern int players_max_points();
/*0040cfa0*/ extern int round_points_clamp_to_limits();
/*006592e0*/ extern undefined g_kc_panel_6592e0;
/*006592f4*/ extern undefined g_kc_panel_6592f4;
/*00659308*/ extern undefined g_kc_panel_659308;
/*00659318*/ extern undefined4 g_kc_panel_659318;
/*00659328*/ extern undefined g_kc_panel_659328;
/*00659340*/ extern undefined g_kc_659340;
/*00659790*/ extern undefined4 g_kc_659790;

#define KC_ENTRY_STRIDE (0x114)
#define KC_ENTRY_FLAG_OFFSET (0x110)


/* kc_panel_update */

/* "players_reached_target / points / zero points" */

/* Per-frame refresh of the KC panel. First asks the player/round helpers to fill the
   panel's global readouts (target reached, max points, clamped round points, zero-points
   state, points). Then calls kc_panel_entry_update for every entry in the array at
   g_kc_659340 whose flag (KC_ENTRY_FLAG_OFFSET) is non-zero. Finally bumps the int at
   g_kc_659790 (inferred: a counter, stored just past the end of the entry array).
   No parameters, no return value. */
static int kc_panel_entry_update_r(int p0);
extern void kc_panel_entry_update(void);
void kc_panel_update(void)

{
  undefined *entry;

  players_reached_target(&g_kc_panel_659308);
  players_max_points(&g_kc_panel_659328,0);
  round_points_clamp_to_limits(&g_kc_panel_6592f4);
  players_have_zero_points(&g_kc_panel_6592e0);
  players_get_points(&g_kc_panel_659318);
  /* Walk the entry array in KC_ENTRY_STRIDE steps, from g_kc_659340 up to the end
     marker &g_kc_659790; update each entry whose flag is set. */
  entry = &g_kc_659340;
  do {
    if (*(int *)(entry + KC_ENTRY_FLAG_OFFSET) != 0) {
      kc_panel_entry_update_r(entry);
    }
    entry = entry + KC_ENTRY_STRIDE;
  } while ((LONG_PTR)entry < ((LONG_PTR)&g_kc_659790));
  g_kc_659790 = g_kc_659790 + 1;
  return;
}

/* kc_panel_entry_update_r: C -> original kc_panel_entry_update (registers esi) */
__declspec(naked) int kc_panel_entry_update_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call kc_panel_entry_update
        pop esi
        ret
    }
}
