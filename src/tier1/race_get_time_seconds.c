#include "mfl_tier2.h"
/*005dda1c*/ extern float k_f_3_3333333en4;
/*00652de8*/ extern float g_race_652de8;
/*007ef7d8*/ extern undefined4 g_camera_7ef7d8;
/* race_get_time_seconds */
/* Returns the race time in seconds (inferred): g_camera_7ef7d8 scaled by k_f_3_3333333en4 (~1/3000, inferred),
 * minus g_race_652de8 (inferred: the phase start time).
 * (was: "g_race_time_ticks / 3000 minus the phase start time"; the body never reads g_race_time_ticks) */
double race_get_time_seconds(void)
{
  return (double)k_f_3_3333333en4 * (double)(int)g_camera_7ef7d8 - (double)g_race_652de8;
}
