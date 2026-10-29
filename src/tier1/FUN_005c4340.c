#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall CreateSemaphoreA(int, int, int, int);
/* Creates an unnamed Win32 semaphore (inferred from the CreateSemaphoreA call). */
/* Creates an unnamed Win32 semaphore (inferred from the CreateSemaphoreA call).
 * Parameters:
 *   semaphore_out - receives the semaphore HANDLE (written even on failure)
 *   initial_count - CreateSemaphoreA lInitialCount
 *   max_count     - CreateSemaphoreA lMaximumCount
 * Returns semaphore_out on success, or NULL if CreateSemaphoreA failed.
 */
uint FUN_005c4340(undefined4 *semaphore_out,LONG initial_count,LONG max_count)
{
  HANDLE semaphore;

  /* security attributes and name are NULL: default security, unnamed semaphore */
  semaphore = CreateSemaphoreA((LPSECURITY_ATTRIBUTES)0x0,initial_count,max_count,(LPCSTR)0x0);
  *semaphore_out = semaphore;
  return (semaphore != (HANDLE)0x0 ? 0xffffffffU : 0U) & (uint)semaphore_out;
}
