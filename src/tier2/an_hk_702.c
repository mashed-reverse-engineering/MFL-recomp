#include "mfl_tier2.h"
extern struct EventLogArgs g_event_log_args[256];
extern struct EventLogData g_event_log_data[256];
/*00470f70*/ extern undefined __cdecl sound_play_impact_voice(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004b2b48*/ extern int _ftol();
/*006ae130*/ extern undefined g_sound_6ae130;
/*007e7f60*/ extern undefined4 g_event_log_head;
/*007e7f64*/ extern undefined4 g_event_log_type;
/*007e8364*/ extern undefined4 g_event_log_time;
/*007e8764*/ extern undefined4 g_event_log_param;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
#define EVENT_LOG_SIZE (256)
#define EVENT_LOG_LAST_INDEX (0xff)
#define EVENT_DATA_STRIDE (64)
/* sound_event_log_process */

/* "walks the event log ring from g_event_log_head and plays impact voices" */

/*
 * Plays the impact voices for events logged this frame.
 * Walks the event log ring backwards from the newest slot (g_event_log_head - 1),
 * at most EVENT_LOG_SIZE entries. An entry is handled only when its time equals
 * g_last_frame_ticks; the walk stops at the first entry with any other time.
 * Takes no parameters and returns nothing.
 */
void sound_event_log_process(void)

{
  float event_value;
  bool walk_done;
  int frame_ticks;
  int mode_flag;
  int remaining;
  int voice_id;
  int event_index;

  frame_ticks = g_last_frame_ticks;
  event_index = g_event_log_head + -1;
  if (event_index < 0) {
    event_index = EVENT_LOG_LAST_INDEX;
  }
  remaining = EVENT_LOG_SIZE;
  walk_done = false;
  /* One pass per ring slot, newest first. */
  do {
    if (walk_done) {
      return;
    }
    if ((&g_event_log_time)[event_index] == frame_ticks) {
      /* voice_id: the logged param with CAR_ID_BASE removed (inferred). */
      voice_id = (&g_event_log_param)[event_index] + -CAR_ID_BASE;
      /* Dispatch on the event type. */
      switch((&g_event_log_type)[event_index]) {
      case 3:
        sound_play_impact_voice(voice_id,0,0,0);
        break;
      case 4:
        sound_play_impact_voice(voice_id,1,g_event_log_args[event_index].arg1,0);
        break;
      case 8:
        /* Float payload w04 is passed on as raw int bits; arg2 picks the voice variant (inferred). */
        event_value = g_event_log_data[event_index].w04;
        switch(g_event_log_args[event_index].arg2) {
        case 0:
          sound_play_impact_voice(voice_id,3,0,*(int *)&event_value);
          break;
        case 1:
          sound_play_impact_voice(voice_id,2,0,*(int *)&event_value);
          break;
        case 3:
          sound_play_impact_voice(voice_id,3,0,*(int *)&event_value);
          break;
        case 4:
          sound_play_impact_voice(voice_id,4,0,*(int *)&event_value);
        }
        break;
      case 0xb:
        /* Float w00 truncated to int; voice 3 plays only when it is 0 or 1 (inferred). */
        mode_flag = ((int)(*(float *)((char *)&g_event_log_data + event_index * EVENT_DATA_STRIDE)));
        if ((mode_flag == 0) || (mode_flag == 1)) {
          sound_play_impact_voice(voice_id,3,0,0);
        }
        break;
      case 0xc:
        /* Sets the per-voice flag word g_sound_6ae130[voice_id] (inferred). */
        ((undefined4 *)&g_sound_6ae130)[voice_id] = 1;
      }
    }
    else {
      walk_done = true;
    }
    event_index = event_index + -1;
    if (event_index < 0) {
      event_index = EVENT_LOG_LAST_INDEX;
    }
    remaining = remaining + -1;
  } while (0 < remaining);
  return;
}


