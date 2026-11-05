/* rwa_soft_output_free_005d8e00 (0x005d8e00): software output: release what the output object holds */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwBool __stdcall CloseHandle(void *handle);
/*005cc3a0*/ extern void FUN_005cc3a0(void *device_output, void *mixer);      /* _RwaDXAudioOutputSetMixerCallback (NULL = stop) */
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*005baf80*/ extern void *FUN_005baf80(void *obj, int a, int b);      /* RwaObjDestroy */

#define SOFT_OUTPUT_OWNS_DEVICE 1      /* output+0x4c flags */

/* +0x3c = the device output it mixes into, +0x58 = the mix buffer, +0x2c = the mixer semaphore.  Returns CloseHandle's result. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwBool rwa_soft_output_free_005d8e00(char *output)
{
  FUN_005cc3a0(*(void **)(output + 0x3c), 0);
  FUN_005bf120(*(void **)(output + 0x58));
  if (*(unsigned char *)(output + 0x4c) & SOFT_OUTPUT_OWNS_DEVICE) {
    FUN_005baf80(*(void **)(output + 0x3c), 0, 0);
    *(RwUInt32 *)(output + 0x4c) &= ~SOFT_OUTPUT_OWNS_DEVICE;
  }
  return CloseHandle(*(void **)(output + 0x2c));
}
