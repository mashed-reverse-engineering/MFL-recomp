#include "mfl_tier2.h"
/*0069cbd4*/ extern undefined4 g_event_type;


/* event_type_uses_standard_menu */

/* "false for event types 6..10 and 0x12, true for the others" */

undefined4 event_type_uses_standard_menu(void)
{
  if (((((g_event_type != EVENT_TURBO_TOURNAMENT) && (g_event_type != EVENT_ICE_TOURNAMENT)) && (g_event_type != EVENT_NIGHT_TOURNAMENT)) &&
      (((g_event_type != EVENT_TRAINING && (g_event_type != EVENT_TIME_TRIAL_0)) &&
       ((g_event_type != EVENT_TIME_TRIAL_1 && ((g_event_type != EVENT_TIME_TRIAL_2 && (g_event_type != EVENT_QUICK_RACE_11)))))))) &&
     ((g_event_type != EVENT_QUICK_RACE_12 &&
      ((((g_event_type != EVENT_QUICK_RACE_13 && (g_event_type != EVENT_CUP_3)) && (g_event_type != EVENT_CUP_4)) && (g_event_type != EVENT_CUP_5)
       ))))) {
    return 0;
  }
  return 1;
}
