#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall InterlockedExchange(int, int);
/* FUN_005c4440 */

/* Atomically stores value into *target (InterlockedExchange) and returns target.
   target: pointer to the LONG to overwrite. value: new value for it.
   Returns target itself. */
LONG * __cdecl FUN_005c4440(LONG *target,LONG value)

{
  InterlockedExchange(target,value);
  return target;
}


