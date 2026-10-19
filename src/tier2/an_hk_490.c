#include "mfl_tier2.h"
extern struct EventLogArgs g_event_log_args[256];
/*0040fdc0*/ extern int race_get_phase();
/*00468ac0*/ extern int voice_queue_add();
/*007e7f60*/ extern undefined4 g_event_log_head;
/*007e7f64*/ extern undefined4 g_event_log_type;
/*007e8364*/ extern undefined4 g_event_log_time;
/*007e8764*/ extern undefined4 g_event_log_param;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
#define EVENT_LOG_SIZE (0x100)
#define EVENT_LOG_LAST_INDEX (0xff)
#define EVENT_ID_BASE (0xff00) /* (inferred) event ids are stored offset by this */
#define EVENT_TYPE_VOICE_A (0x3ff03) /* (inferred) */
#define EVENT_TYPE_VOICE_B (0x3ff04) /* (inferred) */
#define VOICE_QUEUE_KIND (0x13) /* (inferred) first argument of voice_queue_add */
/* sound_event_log_find_current_tick */

/* "scans the event log back from the head for entries with the current tick" */

/*
 * Walks the sound event ring buffer backwards from the newest entry (g_event_log_head - 1).
 * While entries carry the last frame's tick, a voice-type entry (EVENT_TYPE_VOICE_A/B) queues
 * two voice_queue_add calls with its param and arg ids swapped. Stops at the first entry with
 * a different tick, or after EVENT_LOG_SIZE entries. Takes no parameters, returns nothing.
 * (inferred: the swapped pair is meant to play the sound for both ids)
 */
void sound_event_log_find_current_tick(void)

{
  int event_param;
  bool scan_done;
  int current_tick;
  int log_index;
  int event_arg;
  int entries_left;

  race_get_phase();
  current_tick = g_last_frame_ticks;
  log_index = g_event_log_head + -1;
  if (log_index < 0) {
    log_index = EVENT_LOG_LAST_INDEX;
  }
  entries_left = EVENT_LOG_SIZE;
  scan_done = false;
  /* scan backwards one entry per pass; scan_done makes the next pass return */
  do {
    if (scan_done) {
      return;
    }
    if ((&g_event_log_time)[log_index] == current_tick) {
      event_param = (&g_event_log_param)[log_index];
      event_arg = g_event_log_args[log_index].arg0 - EVENT_ID_BASE;
      /* voice entries: queue both orderings of the id pair */
      if (((&g_event_log_type)[log_index] == EVENT_TYPE_VOICE_A) || ((&g_event_log_type)[log_index] == EVENT_TYPE_VOICE_B)) {
        voice_queue_add(VOICE_QUEUE_KIND,event_param + -EVENT_ID_BASE,event_arg);
        voice_queue_add(VOICE_QUEUE_KIND,event_arg,event_param + -EVENT_ID_BASE);
      }
    }
    else {
      /* older tick reached: this is the end of the current frame's entries */
      scan_done = true;
    }
    /* step back, wrapping from index 0 to the last slot of the ring */
    log_index = log_index + -1;
    if (log_index < 0) {
      log_index = EVENT_LOG_LAST_INDEX;
    }
    entries_left = entries_left + -1;
  } while (0 < entries_left);
  return;
}


