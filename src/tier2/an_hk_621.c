#include "mfl_tier2.h"
/*004a2790*/ extern int app_frame_timing();
/*005dda1c*/ extern float k_f_3_3333333en4;
/*0076e860*/ extern undefined4 g_app_frame_76e860;
/*007ef7e8*/ extern undefined4 g_frame_ticks;
/*007ef7ec*/ extern float g_frame_seconds;

/* Frame ticks per timing step, and the raw tick windows that snap to each step count (inferred from the ranges) */
#define TICKS_PER_STEP (0x32)
#define TICKS_NEAR_50_MIN (0x2f)
#define TICKS_NEAR_50_MAX (0x35)
#define TICKS_NEAR_100_MIN (0x61)
#define TICKS_NEAR_100_MAX (0x67)
#define TICKS_NEAR_150_MIN (0x93)
#define TICKS_NEAR_150_MAX (0x99)
#define TICKS_NEAR_200_MIN_EXCL (0xc4)
#define TICKS_NEAR_200_MAX_EXCL (0xcc)
/* app_frame_wait */

/* "waits until the frame delta is reached"
 * Per frame: samples the raw tick count via app_frame_timing(), snaps it to the nearest
 * 50/100/150/200 step (inferred), adds it to the g_app_frame_76e860 accumulator, and
 * converts whole TICKS_PER_STEP units out of the accumulator into g_frame_ticks (ticks)
 * and g_frame_seconds (seconds). Takes no parameters; always returns 1.
 * (inferred: the accumulator's remainder is kept, or dropped when it is tiny) */

undefined4 app_frame_wait(void)

{
  uint step_count;

  app_frame_timing();
  /* snap the raw tick count into one of the step windows; outside every window it is kept as is */
  if (((int)g_frame_ticks < TICKS_NEAR_50_MIN) || (TICKS_NEAR_50_MAX < (int)g_frame_ticks)) {
    if (((int)g_frame_ticks < TICKS_NEAR_100_MIN) || (TICKS_NEAR_100_MAX < (int)g_frame_ticks)) {
      if (((int)g_frame_ticks < TICKS_NEAR_150_MIN) || (TICKS_NEAR_150_MAX < (int)g_frame_ticks)) {
        if ((TICKS_NEAR_200_MIN_EXCL < (int)g_frame_ticks) && ((int)g_frame_ticks < TICKS_NEAR_200_MAX_EXCL)) {
          g_frame_ticks = 200;
        }
      }
      else {
        g_frame_ticks = 0x96;
      }
    }
    else {
      g_frame_ticks = 100;
    }
  }
  else {
    g_frame_ticks = TICKS_PER_STEP;
  }
  g_app_frame_76e860 = g_app_frame_76e860 + g_frame_ticks;
  /* split the accumulator into whole steps (step_count) and the leftover ticks */
  step_count = 0;
  if (0x31 < (int)g_app_frame_76e860) {
    step_count = g_app_frame_76e860 / TICKS_PER_STEP;
    g_app_frame_76e860 = g_app_frame_76e860 % TICKS_PER_STEP;
  }
  /* leftover >= 48 ticks rounds up to one more step; leftover of 3..47 is kept for the next frame;
     leftover of 2 or less is dropped */
  if ((int)g_app_frame_76e860 < 0x30) {
    if (2 < (int)g_app_frame_76e860) goto LAB_004a2911;
  }
  else {
    step_count = step_count + 1;
  }
  g_app_frame_76e860 = 0;
LAB_004a2911:
  /* publish the step count as ticks and as seconds */
  g_frame_ticks = step_count * TICKS_PER_STEP;
  g_frame_seconds = (float)(int)(step_count * TICKS_PER_STEP) * k_f_3_3333333en4;
  return 1;
}


