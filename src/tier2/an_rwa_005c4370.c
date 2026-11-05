/* rwa_semaphore_poll_005c4370 (0x005c4370): _rwaSemaphorePoll, take the semaphore if it is free, without waiting */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);

#define WAIT_TIMEOUT_ 0x102

/* The semaphore object holds the Win32 handle in its first word.  1 = taken, 0 = busy. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_semaphore_poll_005c4370(void **semaphore)
{
  return WaitForSingleObject(*semaphore, 0) != WAIT_TIMEOUT_;
}
