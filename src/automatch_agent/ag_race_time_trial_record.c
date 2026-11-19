#include "mfl_tier2.h"
/*00408e60*/ extern int player_record_get_value();
/*004138a0*/ extern int time_split();
/*00434bb0*/ extern int get_event_type();
/*0047b520*/ extern int car_current_matrix();
/*00494520*/ extern undefined __cdecl replay_buffer_record_frame(int buffer, RwMatrix *matrix, undefined4 value, int sample_index);
/*004a8420*/ extern int debug_printf();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *str, ...);
/*00652f88*/ extern undefined4 g_replay_652f88;
/*00652f8c*/ extern undefined4 g_replay_652f8c;
/*00652f98*/ extern undefined4 g_replay_652f98;
/*007ef7dc*/ extern undefined4 g_race_clock_a;


/* race_time_trial_record */

/* "'Time trial recording error time %d'" */

void race_time_trial_record(undefined4 ticks)

{
  undefined4 split_ticks;
  int event_type;
  float time_fraction;
  undefined4 time_seconds;
  undefined4 time_minutes;
  RwMatrix *car_matrix;
  
  event_type = get_event_type();
  if ((event_type >= 0) && (event_type <= 2)) {
    if (g_replay_652f8c == g_replay_652f88) {
      debug_printf("Time trial recording error time %d\n",g_race_clock_a);
    }
    car_current_matrix(&car_matrix,0);
    replay_buffer_record_frame(g_replay_652f8c,car_matrix,ticks,0xf);
    g_replay_652f98 = 0;
    event_type = player_record_get_value(0,0);
    if ((event_type != 0) && (*(int *)(g_replay_652f8c + 0x17c) == 0)) {
      *(undefined4 *)(g_replay_652f8c + 0x17c) = ticks;
      split_ticks = *(undefined4 *)(g_replay_652f8c + 0x17c);
      time_split(split_ticks,&time_fraction,&time_seconds,&time_minutes);
      FID_conflict__wprintf("Hit intermediate 0 %d\n",split_ticks);
      FID_conflict__wprintf("Hit intermediate 0 %d:%d:%f\n",time_minutes,time_seconds,(double)time_fraction);
    }
    event_type = player_record_get_value(0,1);
    if ((event_type != 0) && (*(int *)(g_replay_652f8c + 0x180) == 0)) {
      *(undefined4 *)(g_replay_652f8c + 0x180) = ticks;
      split_ticks = *(undefined4 *)(g_replay_652f8c + 0x180);
      time_split(split_ticks,&time_fraction,&time_seconds,&time_minutes);
      FID_conflict__wprintf("Hit intermediate 1 %d\n",split_ticks);
      FID_conflict__wprintf("Hit intermediate 1 %d:%d:%f\n",time_minutes,time_seconds,(double)time_fraction);
    }
  }
  return;
}
