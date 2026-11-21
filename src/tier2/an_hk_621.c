#include "mfl_tier2.h"
#include "mfl_fix.h"
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

#if MFL_FIX_FIXED_STEP
/*004a41f0*/ extern int timer_ticks(void);
/*007ef7d8*/ extern int g_camera_7ef7d8;
/*00652f68*/ extern int g_race_phase;
/*0076e7f4*/ extern int g_app_state;
#define TIMER_TICKS_PER_STEP 50000      /* timer_ticks() runs at 3,000,000 per second: one 1/60 s step */
#define STALL_TICKS 750000              /* a frame longer than this (250 ms: load, debugger) counts as one step */
#define MAX_STEPS 8                     /* at most this many steps in one frame (below 7.5 fps: slow motion, no spiral of ever longer frames) */

/* MFL_FIX_FIXED_STEP: the steps of the next frame from the real time since the last call, rounded to the nearest step with the rest carried
   (|carry| <= half a step), so the long-run game speed is real time at any frame rate and a steady 60 fps gives one step every frame. */
static uint fixed_step_count(void)
{
  static int last, carry, started, last_phase, last_app_state;
  int now = timer_ticks(), delta = now - last, steps;

  last = now;
  if (!started || delta < 0 || delta > STALL_TICKS || g_race_phase != last_phase || g_app_state != last_app_state) {
    /* first frame, stall, or a state change (a course load, a phase start): one step as the original, nothing carried, so a load does
       not run catch-up steps (and a catch-up does not move the frames of the sound and input work) */
    started = 1;
    carry = 0;
    last_phase = g_race_phase;
    last_app_state = g_app_state;
    return 1;
  }
  carry = carry + delta;
  steps = (carry + TIMER_TICKS_PER_STEP / 2) / TIMER_TICKS_PER_STEP;
  carry = carry - steps * TIMER_TICKS_PER_STEP;
  if (steps > MAX_STEPS) {
    steps = MAX_STEPS;
    carry = 0;
  }
  return (uint)steps;
}
#endif

undefined4 app_frame_wait(void)

{
  uint step_count;

  app_frame_timing();
#if MFL_FIX_FIXED_STEP
  step_count = fixed_step_count();
  g_camera_7ef7d8 = g_camera_7ef7d8 + (int)(step_count * TICKS_PER_STEP) - TICKS_PER_STEP;      /* the game-time counter: +50 per step, not per frame */
  goto publish_step_time;
#endif
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
    if (2 < (int)g_app_frame_76e860) goto publish_step_time;
  }
  else {
    step_count = step_count + 1;
  }
  g_app_frame_76e860 = 0;
publish_step_time:
  /* publish the step count as ticks and as seconds */
  g_frame_ticks = step_count * TICKS_PER_STEP;
  g_frame_seconds = (float)(int)(step_count * TICKS_PER_STEP) * k_f_3_3333333en4;
  return 1;
}


