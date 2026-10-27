#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase();
/*00434bb0*/ extern int get_event_type();
/*00468780*/ extern int sound_tick_counter_increment();
/*004692f0*/ extern int voice_queue_race_update();
/*004720c0*/ extern int voice_queue_play_pending();
/*008bed00*/ extern undefined4 g_sound_frame_8bed00;
/*008d98bc*/ extern int g_sound_player_count;


/* voice_queue_frame_update */

/* "per frame entry: tick counter, race queue update (not for event types 0 and 1), player updates
   and pending lines" */

void voice_queue_frame_update(void)
{
  int event_type;
  
  sound_tick_counter_increment();
  event_type = get_event_type();
  if (event_type != EVENT_TIME_TRIAL_0) {
    event_type = get_event_type();
    if (event_type != EVENT_TIME_TRIAL_1) {
      event_type = get_event_type();
      if (event_type != EVENT_TIME_TRIAL_2) {
        voice_queue_race_update();
        event_type = 0;
        if (0 < g_sound_player_count) {
          do {
            voice_queue_player_update(event_type);
            event_type = event_type + 1;
          } while (event_type < g_sound_player_count);
        }
        voice_queue_play_pending();
        g_sound_frame_8bed00 = race_get_phase();
      }
    }
  }
  return;
}
