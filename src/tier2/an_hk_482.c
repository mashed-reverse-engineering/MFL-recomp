#include "mfl_tier2.h"
extern struct EventLogArgs g_event_log_args[256];
/*007e7f60*/ extern undefined4 g_event_log_head;
/*007e7f64*/ extern undefined4 g_event_log_type;
/*007e8364*/ extern undefined4 g_event_log_time;
/*007e8764*/ extern undefined4 g_event_log_param;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
static int event_log_dispatch_r(int reg_eax);
extern void event_log_dispatch(void);
#define EVENT_LOG_SIZE (256)
#define EVENT_LOG_LAST_INDEX (0xff)
#define EVENT_TYPE_COUNT (0xd)
__declspec(naked) int event_log_dispatch_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call event_log_dispatch
        ret
    }
}

/*
 * event_log_flush_new
 *
 * Walks the event log ring buffer backwards from g_event_log_head and dispatches
 * the entries logged in the current frame (time == g_last_frame_ticks), stopping at
 * the first entry from an earlier frame. Type 6 entries are dispatched at most once
 * per param, tracked in dispatched[] at index (param - CAR_ID_BASE) (inferred),
 * and only when arg0 is one of the two marker values (inferred).
 *
 * Takes no parameters; returns nothing.
 */
void event_log_flush_new(void)

{
  uint event_type;
  int param;
  bool done;
  int frame_ticks;
  int remaining;
  int entry;
  int dispatched [4];
  
  /* start at the most recent entry (head - 1), wrapping at the ring size */
  frame_ticks = g_last_frame_ticks;
  entry = g_event_log_head + -1;
  dispatched[0] = 0;
  dispatched[1] = 0;
  dispatched[2] = 0;
  dispatched[3] = 0;
  if (entry < 0) {
    entry = EVENT_LOG_LAST_INDEX;
  }
  remaining = EVENT_LOG_SIZE;
  done = false;
  /* walk backwards over at most one full ring of entries */
  do {
    if (done) {
      return;
    }
    /* entries older than this frame end the walk */
    if ((&g_event_log_time)[entry] == frame_ticks) {
      event_type = (&g_event_log_type)[entry];
      /* type 0 and types >= EVENT_TYPE_COUNT are ignored */
      if ((event_type != 0) && (event_type < EVENT_TYPE_COUNT)) {
        /* type 6: dispatch once per param, only for the marker arg0 values */
        if (event_type == 6) {
          param = (&g_event_log_param)[entry];
          if (((g_event_log_args[entry].arg0 == 0xff0000ff) ||
              (g_event_log_args[entry].arg0 == 0xffff0080)) && (dispatched[param + -CAR_ID_BASE] == 0)) {
            event_log_dispatch_r(entry);
            dispatched[param + -CAR_ID_BASE] = 1;
          }
        }
        else {
          event_log_dispatch_r(entry);
        }
      }
    }
    else {
      done = true;
    }
    entry = entry + -1;
    if (entry < 0) {
      entry = EVENT_LOG_LAST_INDEX;
    }
    remaining = remaining + -1;
  } while (0 < remaining);
  return;
}


