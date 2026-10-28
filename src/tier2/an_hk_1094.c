#include "mfl_tier2.h"
/* course_storm_cb0_sub_4aac50 */

/* single caller course_storm_cb0 */

/* Initialises a spline walker: spline, start/end/step parameters, current t = start, user arg, no matrix (+0x1c = 0), then runs course_storm_cb0_sub_4aaa60 (inferred). */
void course_storm_cb0_sub_4aac50(undefined4 *spline_state,undefined4 spline,undefined4 t_start,undefined4 t_end,
               undefined4 t_step,undefined4 user_arg)
{
  *spline_state = spline;
  spline_state[2] = t_end;
  spline_state[3] = t_step;
  spline_state[1] = t_start;
  spline_state[4] = t_start;
  spline_state[5] = user_arg;
  spline_state[7] = 0;
  course_storm_cb0_sub_4aaa60(spline_state);
  return;
}


