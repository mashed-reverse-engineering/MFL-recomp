#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase();
/* The event log ring buffer is ONE object in the original: head, then the parallel arrays type / time / param (256 each), then the 32 byte args and
   the 64 byte data records, contiguous from 0x7e7f60.  A store through log.type[log.head] may therefore alias log.head, and the compiler reloads
   the head for every store (the original does); separate globals let the compiler cache the head, which differs from the original when the head
   is out of range (a wild head overlaps the log itself). */
struct EventLogRing {
  uint head;
  uint type[256];
  uint time[256];
  uint param[256];
  struct EventLogArgs args[256];
  struct EventLogData data[256];
};
/*007e7f60*/ extern struct EventLogRing g_event_log_head;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
/*008b06e0*/ extern struct Car player_car[16];
/* event type stored in EventLogRing.type for this log entry (inferred: name by role) */
#define EVENT_TYPE_CAR_STATE (6)
/* added to the car index in EventLogRing.param (car index + 0xff00) (inferred) */
/* constant stored in the first args word (args.arg0) (inferred, role unknown) */
#define EVENT_ARG0_MARKER (0xff0000ff)
/* words per EventLogRing.data record: 16 x 4 = 64 bytes */
#define EVENT_DATA_WORDS (0x10)
/* last valid index of the 256 entry ring (wraps to 0 after it) */
#define EVENT_LOG_LAST_INDEX (0xff)
/* car_log_event_6 */

/* Logs an EVENT_TYPE_CAR_STATE entry into the global event ring g_event_log_head for one car.
   car_index: index into player_car[16]. Nothing is logged unless that car's mode is 0 or 1 and
   race_get_phase() is not 1..5 (inferred: the phases where logging is suppressed). Returns nothing.
   Note: buf is never written here, so the args and data words copied from it are whatever is in
   those stack slots (inferred, check disassembly). */

void __cdecl car_log_event_6(int car_index)

{
  undefined4 frame_ticks;
  int race_phase;
  int next_index;
  uint buf[16];

  if ((player_car[car_index].mode == 0) || (player_car[car_index].mode == 1)) {
    race_phase = race_get_phase();
    frame_ticks = g_last_frame_ticks;
    /* skip logging in race phases 1..5 */
    if (((race_phase != 1) && (((race_phase != 2 && (race_phase != 3)) && (race_phase != 4)))) &&
       (race_phase != 5)) {
      /* fill the header fields and the args record of the current ring slot */
      g_event_log_head.type[g_event_log_head.head] = EVENT_TYPE_CAR_STATE;
      g_event_log_head.time[g_event_log_head.head] = frame_ticks;
      g_event_log_head.param[g_event_log_head.head] = car_index + CAR_ID_BASE;
      g_event_log_head.args[g_event_log_head.head].arg0 = EVENT_ARG0_MARKER;
      g_event_log_head.args[g_event_log_head.head].arg1 = buf[1];
      g_event_log_head.args[g_event_log_head.head].arg2 = buf[2];
      g_event_log_head.args[g_event_log_head.head].w0c = buf[3];
      g_event_log_head.args[g_event_log_head.head].w10 = buf[4];
      g_event_log_head.args[g_event_log_head.head].w14 = buf[5];
      g_event_log_head.args[g_event_log_head.head].w18 = buf[6];
      g_event_log_head.args[g_event_log_head.head].w1c = buf[7];
      /* copy buf[0..15] into the data record of the current slot; race_phase is reused as the loop index */
      race_phase = 0;
      do {
        next_index = race_phase + 1;
        ((uint *)&g_event_log_head.data[g_event_log_head.head].w00)[race_phase] = buf[race_phase];
        race_phase = next_index;
      } while (next_index < EVENT_DATA_WORDS);
      /* advance the ring head, wrapping to slot 0 after the last slot */
      g_event_log_head.head = g_event_log_head.head + 1;
      if (EVENT_LOG_LAST_INDEX < g_event_log_head.head) {
        g_event_log_head.head = 0;
      }
    }
  }
  return;
}
