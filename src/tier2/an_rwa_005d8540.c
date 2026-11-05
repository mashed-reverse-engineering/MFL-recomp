/* rwa_soft_voice_set_wave_005d8540 (0x005d8540): software mixer voice: attach a sample block (NULL = none) */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

#define SOFT_VOICE_WAVE_CHANGED 1      /* voice+0xcc flags */
#define SOFT_VOICE_FLAG_4       4

/* voice+0x9c -> the mixer, whose +0x04 is the semaphore that guards the voices against the mixer thread. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_soft_voice_set_wave_005d8540(char *voice, void *samples)
{
  WaitForSingleObject(*(void **)(*(char **)(voice + 0x9c) + 4), 0xffffffff);
  *(void **)(voice + 0x98) = samples;
  *(RwUInt32 *)(voice + 0xcc) = (*(RwUInt32 *)(voice + 0xcc) & ~SOFT_VOICE_FLAG_4) | SOFT_VOICE_WAVE_CHANGED;
  ReleaseSemaphore(*(void **)(*(char **)(voice + 0x9c) + 4), 1, 0);
}
