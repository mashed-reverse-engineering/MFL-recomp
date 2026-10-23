#include "mfl_tier2.h"
/*004b2b48*/ extern int _ftol();
#define SECONDS_PER_TICK (0.00033333333f) /* (inferred) 1/3000 s per tick */
#define SECONDS_PER_MINUTE (60)
/* time_split */

/* "(ticks, out fraction, out seconds, out minutes): splits a tick count into minutes / seconds /
   fraction" */

void time_split(undefined4 ticks,float *out_fraction,int *out_seconds,int *out_minutes)

{
  int seconds;

  /* whole seconds = ticks * SECONDS_PER_TICK, truncated; out_fraction gets the remainder */
  seconds = ((int)((float)(uint)ticks * SECONDS_PER_TICK));
  *out_seconds = seconds;
  *out_fraction = (float)(((float)(uint)ticks * SECONDS_PER_TICK) - (double)seconds);
  *out_minutes = 0;
  seconds = *out_seconds;
  /* carry every 60 seconds into minutes, leaving out_seconds in 0..59 */
  while (SECONDS_PER_MINUTE - 1 < seconds) {
    *out_minutes = *out_minutes + 1;
    seconds = *out_seconds + -SECONDS_PER_MINUTE;
    *out_seconds = seconds;
  }
  return;
}


