#include "mfl_tier2.h"
/*005ddc28*/ extern float k_f_0;
/*005ddea8*/ extern float k_f_320;
/*005ddeac*/ extern float k_f_n320;
/*005ddeb0*/ extern float k_f_700;
/*00652df0*/ extern undefined4 g_intro_652df0;
/*00652df4*/ extern float g_intro_652df4;
/*00652dfc*/ extern float g_intro_652dfc;
/*007ef7f4*/ extern float g_tick_seconds;

/* values of g_intro_652df0 (0 = idle) */
#define BANNER_STATE_SLIDE_IN (1)
#define BANNER_STATE_HOLD (2)
#define BANNER_STATE_SLIDE_OUT (3)
/* banner y position where the slide-in stops, and the hold time in seconds */
#define BANNER_REST_Y (320.0)
#define BANNER_HOLD_SECONDS (5.0)

/* intro_banner_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "state 1: slides the banner at 700 per second down to 320, then state 2 with a 5 s timer" */

/* Per-frame update of the intro banner: a small state machine driven by g_intro_652df0.
   Takes no parameters and returns nothing; it reads and writes only the globals below.
   State 0 (idle): nothing happens.
   State 1 (slide in): the banner y position (g_intro_652df4) falls at 700 units/s until it
   reaches 320, then the hold timer (g_intro_652dfc) is set to 5 s and the state becomes 2.
   State 2 (hold): the hold timer counts down by g_tick_seconds; at 0 the state becomes 3.
   State 3 (slide out): the banner falls at 700 units/s until it reaches -320, then state 0.
   The `a < b != (a == b)` pattern is Ghidra's rendering of `a <= b`. */

void intro_banner_update(void)

{
  /* state 1: slide in; the banner moves down by speed * elapsed time */
  if (g_intro_652df0 == BANNER_STATE_SLIDE_IN) {
    g_intro_652df4 = g_intro_652df4 - g_tick_seconds * k_f_700;
    /* banner reached its rest position (y <= 320): park it there and start the 5 s hold */
    if (g_intro_652df4 < k_f_320 != (g_intro_652df4 == k_f_320)) {
      g_intro_652df4 = BANNER_REST_Y;
      g_intro_652dfc = BANNER_HOLD_SECONDS;
      g_intro_652df0 = BANNER_STATE_HOLD;
    }
  }
  /* state 2: hold; the timer counts down in real seconds */
  else if (g_intro_652df0 == BANNER_STATE_HOLD) {
    g_intro_652dfc = g_intro_652dfc - g_tick_seconds;
    /* hold time is over (timer <= 0) */
    if (g_intro_652dfc < k_f_0 != (g_intro_652dfc == k_f_0)) {
      g_intro_652df0 = BANNER_STATE_SLIDE_OUT;
      return;
    }
  }
  /* state 3: slide out; the banner keeps moving down until it is fully off the top (y <= -320) (inferred) */
  else if (g_intro_652df0 == BANNER_STATE_SLIDE_OUT) {
    g_intro_652df4 = g_intro_652df4 - g_tick_seconds * k_f_700;
    /* banner is off screen: go idle (inferred) */
    if (g_intro_652df4 < k_f_n320 != (g_intro_652df4 == k_f_n320)) {
      g_intro_652df0 = 0;
      return;
    }
  }
  return;
}


