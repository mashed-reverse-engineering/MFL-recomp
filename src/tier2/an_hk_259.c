#include "mfl_tier2.h"
/*005b6680*/ extern void FUN_005b6680(void);
/*006ae108*/ extern undefined4 g_sound_streams_enabled;
/*006ae10c*/ extern undefined4 g_sound_streams_paused;
/* sound_streams_resume */

/*
 * Resumes the sound streams if they were paused. Does nothing when sound is
 * disabled or when the streams are not paused. Clears g_sound_streams_paused
 * after restarting the streams. Returns 1 (always).
 * FUN_005b6680 is the stream restart call (inferred).
 */

undefined4 sound_streams_resume(void)

{
  /* Only resume when sound is enabled and the streams are currently paused (1). */
  if ((g_sound_streams_enabled != 0) && (g_sound_streams_paused == 1)) {
    FUN_005b6680();
    g_sound_streams_paused = 0;
  }
  return 1;
}


