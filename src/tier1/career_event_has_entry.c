#include "mfl_tier2.h"
enum CareerEventType {
  EVENT_TYPE_UNKNOWN_0 = 0,
  EVENT_TYPE_UNKNOWN_1 = 1,
  EVENT_TYPE_UNKNOWN_2 = 2,
  EVENT_TYPE_UNKNOWN_3 = 3,
  EVENT_TYPE_UNKNOWN_4 = 4,
  EVENT_TYPE_UNKNOWN_5 = 5,
  EVENT_TYPE_UNKNOWN_6 = 6,
  EVENT_TYPE_UNKNOWN_7 = 7,
  EVENT_TYPE_UNKNOWN_8 = 8,
  EVENT_TYPE_UNKNOWN_11 = 0xb,
  EVENT_TYPE_UNKNOWN_12 = 0xc,
  EVENT_TYPE_UNKNOWN_13 = 0xd,
  EVENT_TYPE_UNKNOWN_14 = 0xe,
  EVENT_TYPE_UNKNOWN_15 = 0xf,
  EVENT_TYPE_UNKNOWN_16 = 0x10,
  EVENT_TYPE_UNKNOWN_17 = 0x11
};

/*0069cbd4*/ extern undefined4 g_event_type;


/* career_event_has_entry */

/* Checks if current career event type has an entry, returning the event type. */
/* "switch on event type returning 1 for the listed events" */

undefined4 career_event_has_entry(void)
{
  undefined4 event_type;
  
  event_type = g_event_type;
  switch(g_event_type) {
  case EVENT_TYPE_UNKNOWN_0:
    return EVENT_TYPE_UNKNOWN_0;
  case EVENT_TYPE_UNKNOWN_1:
    return EVENT_TYPE_UNKNOWN_1;
  case EVENT_TYPE_UNKNOWN_2:
    return EVENT_TYPE_UNKNOWN_2;
  case EVENT_TYPE_UNKNOWN_3:
    return EVENT_TYPE_UNKNOWN_3;
  case EVENT_TYPE_UNKNOWN_4:
    return EVENT_TYPE_UNKNOWN_4;
  case EVENT_TYPE_UNKNOWN_5:
    return EVENT_TYPE_UNKNOWN_5;
  case EVENT_TYPE_UNKNOWN_6:
    return EVENT_TYPE_UNKNOWN_6;
  case EVENT_TYPE_UNKNOWN_7:
    return EVENT_TYPE_UNKNOWN_7;
  case EVENT_TYPE_UNKNOWN_8:
    return EVENT_TYPE_UNKNOWN_8;
  case EVENT_TYPE_UNKNOWN_11:
    return EVENT_TYPE_UNKNOWN_11;
  case EVENT_TYPE_UNKNOWN_12:
    return EVENT_TYPE_UNKNOWN_12;
  case EVENT_TYPE_UNKNOWN_13:
    return EVENT_TYPE_UNKNOWN_13;
  case EVENT_TYPE_UNKNOWN_14:
    return EVENT_TYPE_UNKNOWN_14;
  case EVENT_TYPE_UNKNOWN_15:
    return EVENT_TYPE_UNKNOWN_15;
  case EVENT_TYPE_UNKNOWN_16:
    return EVENT_TYPE_UNKNOWN_16;
  case EVENT_TYPE_UNKNOWN_17:
    /* Falls through to common return for event type 17 */
    event_type = EVENT_TYPE_UNKNOWN_17;
  }
  return event_type;
}
