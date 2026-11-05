/* rwa_soft_voice_remove_005d8880 (0x005d8880): software mixer voice: take the voice off the mixer and drop its wave */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

#define SOFT_VOICE_WAVE_CHANGED 1      /* voice+0xcc flags */
#define SOFT_VOICE_FLAG_4       4

/* The mixer's voice list is circular and doubly linked through voice+0xd0 { next, prev } (the mixer walks it from its +0x1c).
   Two separate lock sections, as in the original: unlink, then clear the wave.  Returns the last ReleaseSemaphore result. */
typedef struct RwaSoftLink { struct RwaSoftLink *next, *prev; } RwaSoftLink;

#define MIXER_LOCK(voice) (*(void **)(*(char **)((voice) + 0x9c) + 4))

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwBool rwa_soft_voice_remove_005d8880(char *voice)
{
  RwaSoftLink *link = (RwaSoftLink *)(voice + 0xd0);
  RwBool released;

  WaitForSingleObject(MIXER_LOCK(voice), 0xffffffff);
  link->prev->next = link->next;
  link->next->prev = link->prev;
  ReleaseSemaphore(MIXER_LOCK(voice), 1, 0);
  WaitForSingleObject(MIXER_LOCK(voice), 0xffffffff);
  *(void **)(voice + 0x98) = 0;
  *(RwUInt32 *)(voice + 0xcc) = (*(RwUInt32 *)(voice + 0xcc) & ~SOFT_VOICE_FLAG_4) | SOFT_VOICE_WAVE_CHANGED;
  released = ReleaseSemaphore(MIXER_LOCK(voice), 1, 0);
  *(int *)(voice + 0x7c) = 0;
  return released;
}
