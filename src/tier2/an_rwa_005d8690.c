/* rwa_soft_voice_set_trigger_param_005d8690 (0x005d8690): software mixer voice: parameter callback "trigger" */
#include "mfl_tier2.h"

/*005bb5d0*/ extern RwUInt32 FUN_005bb5d0(void);      /* RwaTimerGetValue */

#define SOFT_VOICE_STOPPED 1      /* voice+0x28 */
#define SOFT_VOICE_PLAYING 2

/* Same as rwa_soft_voice_set_trigger_005d84f0 with the trigger read from *value; returns the voice. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_soft_voice_set_trigger_param_005d8690(char *voice, int param, const int *value)
{
  int trigger = *value;

  (void)param;
  *(int *)(voice + 0x90) = 0;
  *(int *)(voice + 0x94) = 0;
  *(RwUInt32 *)(voice + 0xa0) = FUN_005bb5d0();
  *(int *)(voice + 0x28) = trigger ? SOFT_VOICE_PLAYING : SOFT_VOICE_STOPPED;
  return voice;
}
