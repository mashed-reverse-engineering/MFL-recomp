/* rwa_soft_voice_get_playing_param_005d86d0 (0x005d86d0): software mixer voice: parameter callback "is playing" */
#include "mfl_tier2.h"

/* voice+0x28 = state: 2 and 4 count as playing.  Writes 1 / 0 to *out, returns the voice. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_soft_voice_get_playing_param_005d86d0(char *voice, int param, int *out)
{
  int state = *(int *)(voice + 0x28);

  (void)param;
  *out = (state == 2 || state == 4) ? 1 : 0;
  return voice;
}
