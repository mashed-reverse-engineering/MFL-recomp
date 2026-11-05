/* rwa_soft_voice_set_wave_param_005d8790 (0x005d8790): software mixer voice: parameter callback "wave" */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

#define SOFT_VOICE_WAVE_CHANGED 1      /* voice+0xcc flags */
#define SOFT_VOICE_FLAG_4       4

/* Same as rwa_soft_voice_set_wave_005d8540 (the sample block is the parameter value itself); returns the voice. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_soft_voice_set_wave_param_005d8790(char *voice, int param, void *samples)
{
  (void)param;
  WaitForSingleObject(*(void **)(*(char **)(voice + 0x9c) + 4), 0xffffffff);
  *(void **)(voice + 0x98) = samples;
  *(RwUInt32 *)(voice + 0xcc) = (*(RwUInt32 *)(voice + 0xcc) & ~SOFT_VOICE_FLAG_4) | SOFT_VOICE_WAVE_CHANGED;
  ReleaseSemaphore(*(void **)(*(char **)(voice + 0x9c) + 4), 1, 0);
  return voice;
}
