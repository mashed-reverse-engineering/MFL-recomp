/* rwa_voice_manager_update_time_stamp_005b75b0 (0x005b75b0): voice manager (_rwaVoiceManagerUpdateTimeStamp): advance the millisecond clock by the timer ticks since the last update, x87 */
#include "mfl_tier2.h"

/*005bb5d0*/ extern RwUInt32 rwa_timer_get_value_005bb5d0(void);      /* RwaTimerGetValue */
/*005bb600*/ extern RwUInt32 FUN_005bb600(void);      /* RwaTimerGetFrequency (ticks per second) */

/*007dab90*/ extern RwInt32 g_data_7dab90;      /* the time stamp (ms) */
/*007dab94*/ extern RwUInt32 g_data_7dab94;     /* timer value of the last update */

#define INT32_INDEFINITE (-2147483647 - 1)      /* what a 32-bit fistp stores for NaN and out-of-range values */
/* The original converts with a 32-bit fistp under chop rounding (v: a float variable). */
#define CHOP(v) (((v) >= -2147483648.0f && (v) < 2147483648.0f) ? (int)(v) : INT32_INDEFINITE)

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_voice_manager_update_time_stamp_005b75b0(void)
{
  RwUInt32 now = rwa_timer_get_value_005bb5d0();
  RwUInt32 freq = FUN_005bb600();
  RwUInt32 ticks = now - g_data_7dab94;
  float ms;

  if (now < g_data_7dab94)
    ticks--;      /* (as the original: one tick less across a wrap) */
  ms = (float)ticks / (float)freq * 1000.0f;
  g_data_7dab94 = now;
  g_data_7dab90 += CHOP(ms);
}
