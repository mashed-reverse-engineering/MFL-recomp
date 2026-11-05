/* rwa_soft_voice_init_005d8170 (0x005d8170): software mixer voice: initialise the voice and put it on the mixer's voice list */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
/*005bb5d0*/ extern RwUInt32 FUN_005bb5d0(void);      /* RwaTimerGetValue */

typedef struct RwaSoftLink { struct RwaSoftLink *next, *prev; } RwaSoftLink;      /* voice+0xd0, mixer+0x1c = list head */

#define SOFT_VOICE_IDLE 3      /* voice+0x28 */

/* params+0x20 = the mixer: +0x04 its semaphore, +0x18 output rate (Hz), +0x1c voice list, +0x30 default for voice+0x7c.
   Gains / pitch 1.0, 3d distance 10000, three 0xff levels, everything else 0, trigger time = now.  Returns the voice. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_soft_voice_init_005d8170(char *voice, char *params)
{
  char *mixer = *(char **)(params + 0x20);
  RwaSoftLink *link = (RwaSoftLink *)(voice + 0xd0);
  RwaSoftLink *head = (RwaSoftLink *)(mixer + 0x1c);

  *(float *)(voice + 0x34) = (float)*(RwUInt32 *)(mixer + 0x18);
  *(float *)(voice + 0x3c) = 1.0f;
  *(float *)(voice + 0x40) = 1.0f;
  *(float *)(voice + 0x38) = 1.0f;
  *(float *)(voice + 0x44) = 1.0f;
  *(float *)(voice + 0x64) = 1.0f;
  *(int *)(voice + 0x48) = 0;
  *(float *)(voice + 0x68) = 10000.0f;
  *(int *)(voice + 0x4c) = 0;
  *(int *)(voice + 0x50) = 0;
  *(int *)(voice + 0x54) = 0;
  *(int *)(voice + 0x58) = 0;
  *(int *)(voice + 0x5c) = 0;
  *(int *)(voice + 0x60) = 0;
  *(int *)(voice + 0x28) = SOFT_VOICE_IDLE;
  *(int *)(voice + 0x2c) = 0;
  *(char **)(voice + 0x9c) = mixer;
  *(void **)(voice + 0x98) = 0;
  *(int *)(voice + 0x88) = 0;
  *(int *)(voice + 0x8c) = 0;
  *(int *)(voice + 0x90) = 0;
  *(int *)(voice + 0x94) = 0;
  *(int *)(voice + 0x80) = 0;
  *(int *)(voice + 0x84) = 0;
  *(int *)(voice + 0x6c) = 0xff;
  *(int *)(voice + 0x70) = 0xff;
  *(int *)(voice + 0x74) = 0xff;
  *(int *)(voice + 0x78) = 0;
  *(int *)(voice + 0x7c) = *(int *)(mixer + 0x30);
  *(RwUInt32 *)(voice + 0xa0) = FUN_005bb5d0();
  *(int *)(voice + 0xa4) = 0;
  *(int *)(voice + 0xb0) = 0;
  *(int *)(voice + 0xb4) = 0;
  *(int *)(voice + 0xa8) = 0;
  *(int *)(voice + 0xac) = 0;
  *(int *)(voice + 0xb8) = 0;
  *(int *)(voice + 0xc4) = 0;
  *(int *)(voice + 0xc8) = 0;
  *(int *)(voice + 0xbc) = 0;
  *(int *)(voice + 0xc0) = 0;
  *(RwUInt32 *)(voice + 0xcc) = 0;

  WaitForSingleObject(*(void **)(mixer + 4), 0xffffffff);
  link->prev = 0;
  link->next = 0;
  link->next = head->next;      /* insert at the front */
  link->prev = head;
  head->next->prev = link;
  head->next = link;
  ReleaseSemaphore(*(void **)(mixer + 4), 1, 0);
  return voice;
}
