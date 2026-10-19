#include "mfl_tier2.h"
/*0041b810*/ extern int race_mark_ai_slots();
/*004266c0*/ extern int race_events_process_log(undefined4 p0);
/*00427670*/ extern int ai_editor_update();
/*00428a20*/ extern int cars_step_race_phase_and_respawn(undefined4 p0, undefined4 *p1);
/*00428e30*/ extern undefined __cdecl player_stats_update(undefined4 p0);
/*0047f300*/ extern int cars_step(undefined4 p0, undefined4 *p1);
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f023c*/ extern undefined4 g_ai_editor_mode;
/*007f0258*/ extern undefined4 g_ai_editor_step_flag_a;
/*007f025c*/ extern undefined4 g_ai_editor_step_flag_b;
/*007f0260*/ extern undefined4 g_ai_editor_step_counter;
/*00801b0c*/ extern undefined4 g_car_801b0c;
/* race_update_cars_and_stats */

/* "ai_editor_update, race_mark_ai_slots, cars_step, race_events_process_log, player_stats_update"
    */

undefined4 __cdecl race_update_cars_and_stats(undefined4 param_1)

{
  
  ai_editor_update();
  if (g_player_slots[3].extra == 1) {
    if ((g_ai_editor_mode != 4) ||
       ((g_ai_editor_step_flag_b == 0 && (g_ai_editor_step_flag_a == 0)))) {
      return 0;
    }
    g_ai_editor_step_counter = g_ai_editor_step_counter + 1;
  }
  race_mark_ai_slots();
  cars_step_race_phase_and_respawn(param_1,&g_car_801b0c);
  cars_step(param_1,&g_car_801b0c);
  race_events_process_log(param_1);
  player_stats_update(param_1);
  return 1;
}


