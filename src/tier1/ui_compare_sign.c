#include "mfl_tier2.h"
/* ui_format_number_string */
/* "formats a number below 10 with one format string, else with another" */
#define HUNDREDTHS_PER_SECOND (100)
#define SECONDS_PER_MINUTE (60)
#define HUNDREDTHS_PER_MINUTE (6000)
#define COMPARE_GREATER (0)
#define COMPARE_LESS (1)
#define COMPARE_EQUAL (2)
/* ui_compare_sign */
/* Compares two times (inferred: hundredths of a second, m:ss.cc) and splits the absolute difference
   into minutes, seconds and hundredths. time_a is reused as a - b. sign_out is COMPARE_GREATER (a > b),
   COMPARE_EQUAL (a == b) or COMPARE_LESS (a < b). Returns nothing. */
void ui_compare_sign(int time_a,int time_b,undefined4 *sign_out,int *minutes_out,int *seconds_out, float *hundredths_out)
{
  int seconds;
  int minutes;

  time_a = time_a - time_b;
  *sign_out = COMPARE_GREATER;
  if (time_a == 0) {
    *sign_out = COMPARE_EQUAL;
  }
  else if (time_a < 0) {
    time_a = -time_a;
    *sign_out = COMPARE_LESS;
  }
  minutes = time_a / HUNDREDTHS_PER_MINUTE;
  *minutes_out = minutes;
  seconds = (time_a - minutes * HUNDREDTHS_PER_MINUTE) / HUNDREDTHS_PER_SECOND;
  *seconds_out = seconds;
  *hundredths_out = (float)(time_a + (*minutes_out * SECONDS_PER_MINUTE + seconds) * -HUNDREDTHS_PER_SECOND);
  return;
}
