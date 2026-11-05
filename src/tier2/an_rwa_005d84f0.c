/* rwa_soft_voice_set_trigger_005d84f0 (0x005d84f0): software mixer voice: start (trigger != 0) or stop */
#include "mfl_tier2.h"

/*005bb5d0*/ extern RwUInt32 FUN_005bb5d0(void);      /* RwaTimerGetValue */

#define SOFT_VOICE_STOPPED 1      /* voice+0x28 */
#define SOFT_VOICE_PLAYING 2

/* Rewinds the voice (+0x90 / +0x94 position and fraction), stamps the trigger time (+0xa0) and sets the state; returns the state. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_soft_voice_set_trigger_005d84f0(char *voice, int trigger)
{
  int state = trigger ? SOFT_VOICE_PLAYING : SOFT_VOICE_STOPPED;

  *(int *)(voice + 0x90) = 0;
  *(int *)(voice + 0x94) = 0;
  *(RwUInt32 *)(voice + 0xa0) = FUN_005bb5d0();
  *(int *)(voice + 0x28) = state;
  return state;
}
