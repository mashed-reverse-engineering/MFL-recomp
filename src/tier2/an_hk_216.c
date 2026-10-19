#include "mfl_tier2.h"
/*00416a30*/ extern int career_cell_index();
/*00416d40*/ extern int ai_bots_assign_slots();
/*00427d20*/ extern int game_get_game_8c8ae0();
/*00434bb0*/ extern int get_event_type();
/*005dd340*/ extern float k_f_0_5;
/*005ddc28*/ extern float k_f_0;
/*007ef7b0*/ extern undefined4 g_car_7ef7b0;
/*007ef7b8*/ extern undefined4 game_mode;
/*008c9998*/ extern undefined4 g_ai_counter_9998;
/*008c999c*/ extern float g_ai_skill_bias;
/*008c99a0*/ extern float g_ai_skill_score;
/*008c9a40*/ extern undefined4 g_ai_career_counter_table;
/* race_ai_slot_assign_or_adjust */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "for non-career events adjusts g_ai_skill_bias by +-0.5 or calls ai_bots_assign_slots" */

/* Per-event AI skill bias update. `event_arg` is the event passed to get_event_type (inferred: event handle).
 * Events whose type is in the excluded set below are ignored. For the others:
 *   - race-style game modes nudge g_ai_skill_bias by -0.5 or +0.5 depending on g_car_7ef7b0 (inferred: player flag);
 *   - other modes call ai_bots_assign_slots(), then add g_ai_skill_score / game_get_game_8c8ae0() (inferred: car count)
 *     to g_ai_skill_bias and reset g_ai_skill_score to 0.
 *   Both paths then bump g_ai_counter_9998 and the per-cell counter g_ai_career_counter_table[career_cell_index()].
 * Returns nothing. */
void __fastcall race_ai_slot_assign_or_adjust(undefined4 event_arg)

{
  int event_type;

  /* event types 0,1,2,6,7,8,0xb,0xc,0xd,0x11,0x12 are excluded */
  event_type = get_event_type(event_arg);
  /* non-excluded events only */
  if ((((((event_type != 0) && (event_type != 1)) && (event_type != 2)) &&
       (((event_type != 0xb && (event_type != 0xc)) &&
        ((event_type != 0xd && ((event_type != 6 && (event_type != 7)))))))) && (event_type != 8))
     && ((event_type != 0x12 && (event_type != 0x11)))) {
    /* race-style game modes (4, 5, 8, 9, 10; inferred): nudge the skill bias by a fixed step */
    if (((game_mode == 4) || (((game_mode == 9 || (game_mode == 5)) || (game_mode == 8)))) ||
       (game_mode == 10)) {
      if (g_car_7ef7b0 == 0) {
        g_ai_skill_bias = g_ai_skill_bias - k_f_0_5;
      }
      else {
        g_ai_skill_bias = g_ai_skill_bias + k_f_0_5;
      }
    }
    else {
      /* other modes: assign bot slots, then fold the accumulated score into the bias
         (divided by game_get_game_8c8ae0(), inferred car count, unless that is 0) */
      ai_bots_assign_slots();
      event_type = game_get_game_8c8ae0();
      if ((float)event_type != k_f_0) {
        g_ai_skill_bias = g_ai_skill_score / (float)event_type + g_ai_skill_bias;
      }
      g_ai_skill_score = 0.0;
    }
    /* count this call, then bump the per-career-cell counter (event_type is reused as the cell index here) */
    g_ai_counter_9998 = g_ai_counter_9998 + 1;
    event_type = career_cell_index();
    (&g_ai_career_counter_table)[event_type] = (&g_ai_career_counter_table)[event_type] + 1;
  }
  return;
}


