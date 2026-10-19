#include "mfl_tier2.h"
/*0042ebd0*/ extern double time_get_fraction();
/*0042ebe0*/ extern int time_get_minutes();
/*0042ebf0*/ extern int time_get_seconds();
/*005dd7e8*/ extern float k_f_60;
/*0062ac90*/ extern char *g_ui_62ac90;
/*0062ac94*/ extern char *g_ui_62ac94;
/*0062ac98*/ extern char *g_ui_62ac98;
/*0069d688*/ extern undefined4 g_career_row;
#define CAREER_ROW_SIZE (12) /* bytes per row: 3 floats (inferred) */
#define SECONDS_PER_MINUTE (60)
/* career_time_under_target
 * Returns 1 if the current career time is at or under the time limit of the current career row, else 0.
 * Elapsed time = minutes * 60 + seconds + fraction (time_get_*).
 * Limit = the row's first float (in minutes, * k_f_60 to seconds) + its two other floats (inferred: seconds).
 * Row = g_career_row, CAREER_ROW_SIZE bytes per row (inferred: 3 floats).
 */

undefined4 career_time_under_target(void)

{
  float row_limit_extra_2;
  float row_limit_extra_1;
  float10 row_limit_secs;
  float row_limit_base;
  int minutes;
  int seconds;
  float10 elapsed_secs;

  /* byte offset of the current row; "minutes" is reused for the clock minutes below (same local in the binary) */
  minutes = g_career_row * CAREER_ROW_SIZE;
  row_limit_base = *(float *)((char *)&g_ui_62ac90 + minutes) * k_f_60;
  row_limit_extra_2 = *(float *)((char *)&g_ui_62ac98 + minutes);
  row_limit_extra_1 = *(float *)((char *)&g_ui_62ac94 + minutes);
  minutes = time_get_minutes(0);
  seconds = time_get_seconds(0);
  elapsed_secs = (float10)time_get_fraction(0);
  elapsed_secs = elapsed_secs + (float10)(minutes * SECONDS_PER_MINUTE + seconds);
  row_limit_secs = (float10)(row_limit_base + row_limit_extra_2 + row_limit_extra_1);
  /* same as elapsed_secs <= row_limit_secs (false for NaN) */
  if (elapsed_secs < row_limit_secs != (elapsed_secs == row_limit_secs)) {
    return 1;
  }
  return 0;
}


