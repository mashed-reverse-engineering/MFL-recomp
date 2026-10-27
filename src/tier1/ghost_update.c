#include "mfl_tier2.h"
/*00434bb0*/ extern int get_event_type();
/*00652f8c*/ extern char *g_replay_652f8c;
/*007ef7dc*/ extern undefined4 g_race_clock_a;


/* ghost_update */

/* "event types 0..2: updates the ghost from the best replay (inferred)" */

void ghost_update(void)
{
  int event_type;
  
  event_type = get_event_type();
  if (((event_type != EVENT_TIME_TRIAL_0) && (event_type = get_event_type(), event_type != 1)) &&
     (event_type = get_event_type(), event_type != 2)) {
    return;
  }
  if ((g_replay_652f8c != 0) && (*(int *)(g_replay_652f8c + 0x194) == 0)) {
    g_race_clock_a = 0;
  }
  return;
}
