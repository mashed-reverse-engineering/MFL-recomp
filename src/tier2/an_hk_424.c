#include "mfl_tier2.h"
/*0040ced0*/ extern int players_reached_target();
/*0040cf10*/ extern int players_have_zero_points();
/*0040cf40*/ extern int players_max_points();
/*0040cfa0*/ extern int round_points_clamp_to_limits();
/*0040d000*/ extern int cars_inactive_flags();
/*004b4ba7*/ extern int entry();
/*00658d98*/ extern undefined g_score_658d98;
/*00658dac*/ extern undefined4 g_score_658dac;
/*00658db0*/ extern undefined4 g_score_panel_658db0;
/*00658db4*/ extern undefined4 g_score_panel_658db4;
/*00658db8*/ extern undefined4 g_score_panel_658db8;
/*00658dc0*/ extern undefined4 g_score_panel_658dc0;
/*00658dc4*/ extern undefined g_score_panel_658dc4;
/*00658dd4*/ extern undefined4 g_score_panel_658dd4;
/*00658dd8*/ extern undefined4 g_score_panel_658dd8;
/*00658ddc*/ extern undefined4 g_score_panel_658ddc;
/*00658de0*/ extern undefined4 g_score_panel_658de0;
/*00658de8*/ extern undefined4 g_score_panel_658de8;
/*00658df0*/ extern undefined g_score_panel_658df0;
/*00658fc0*/ extern undefined g_score_panel_658fc0;
/*00658fd0*/ extern undefined4 g_score_panel_658fd0;
#define SCORE_ENTRY_STRIDE (0x74)
#define SCORE_ENTRY_ACTIVE_OFFSET (0x6c)
static int score_panel_entry_update_r(int reg_eax);
extern void score_panel_entry_update(void);
__declspec(naked) int score_panel_entry_update_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call score_panel_entry_update
        ret
    }
}

/*
 * score_panel_update: per-frame refresh of the score panel.
 * Takes no parameters and returns nothing. It calls the race-state helpers (players reached
 * target, max points, clamped round points, zero points, inactive cars), derives the two panel
 * flags g_score_panel_658de8 and g_score_panel_658dc0 from their results (inferred), clears each
 * flag when its guard global is 0, runs score_panel_entry_update for every panel entry that has
 * its active field set, and finally increments the frame counter g_score_panel_658fd0.
 */

void score_panel_update(void)

{
  undefined *panel_entry;

  /* query the race state; the helpers fill the globals passed by address (inferred) */
  players_reached_target(&g_score_panel_658dc4);
  players_max_points(&g_score_panel_658dd4,1);
  round_points_clamp_to_limits(&g_score_658dac);
  players_have_zero_points(&g_score_658d98);
  cars_inactive_flags(&g_score_panel_658fc0);
  /* flags from the results: max points set (de8), round points set (dc0) (inferred) */
  g_score_panel_658de8 = (uint)(g_score_panel_658dd4 != 0);
  g_score_panel_658dc0 = (uint)(g_score_658dac != 0);
  /* guard checks: a zero guard global clears its flag (inferred meaning of each guard) */
  if (g_score_panel_658dd8 == 0) {
    g_score_panel_658de8 = 0;
  }
  if (g_score_panel_658db0 == 0) {
    g_score_panel_658dc0 = 0;
  }
  if (g_score_panel_658ddc == 0) {
    g_score_panel_658de8 = 0;
  }
  if (g_score_panel_658db4 == 0) {
    g_score_panel_658dc0 = 0;
  }
  if (g_score_panel_658de0 == 0) {
    g_score_panel_658de8 = 0;
  }
  if (g_score_panel_658db8 == 0) {
    g_score_panel_658dc0 = 0;
  }
  /* walk the entry table from g_score_panel_658df0 up to g_score_panel_658fc0, one
     entry every SCORE_ENTRY_STRIDE bytes; update each entry whose field at
     SCORE_ENTRY_ACTIVE_OFFSET is non-zero (that field as "active" is inferred) */
  panel_entry = &g_score_panel_658df0;
  do {
    if (*(int *)(panel_entry + SCORE_ENTRY_ACTIVE_OFFSET) != 0) {
      score_panel_entry_update_r(panel_entry);
    }
    panel_entry = panel_entry + SCORE_ENTRY_STRIDE;
  } while ((LONG_PTR)panel_entry < ((LONG_PTR)&g_score_panel_658fc0));
  g_score_panel_658fd0 = g_score_panel_658fd0 + 1;
  return;
}


