/* rwa_soft_output_start_execute_005d8e50 (0x005d8e50): software output: object start-execute under the output's lock */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
/*005bac70*/ extern void *FUN_005bac70(void *obj, int flag);      /* _rwaObjStartExecute */

/* output+0x2c = the semaphore the mixer thread holds while it mixes. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_soft_output_start_execute_005d8e50(char *output)
{
  WaitForSingleObject(*(void **)(output + 0x2c), 0xffffffff);
  FUN_005bac70(output, 1);
  ReleaseSemaphore(*(void **)(output + 0x2c), 1, 0);
  return output;
}
