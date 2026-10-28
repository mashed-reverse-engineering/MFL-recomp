#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall EnterCriticalSection(int);
extern __declspec(dllimport) int __stdcall LeaveCriticalSection(int);
extern __declspec(dllimport) int __stdcall MulDiv(int, int, int);
extern __declspec(dllimport) int __stdcall timeGetTime(void);
/* FUN_004ad8f0 */

/* Stream rate query (inferred): out_rate = MulDiv(100000, iface[+0x44], elapsed_ms) under the critical section; 0 if no time elapsed; E_POINTER if out_rate is NULL. */
undefined4 __stdcall FUN_004ad8f0(char *iface, int *out_rate)

{
  LPCRITICAL_SECTION cs;
  DWORD now_ms;
  int elapsed_ms;
  
  if (out_rate == (int *)0x0) {
    return 0x80004003;
  }
  cs = (LPCRITICAL_SECTION)(iface + -100);
  EnterCriticalSection(cs);
  if (*(int *)(iface + -0x7c) == 0) {
    elapsed_ms = *(int *)(iface + 0x78);
  }
  else {
    now_ms = timeGetTime();
    elapsed_ms = now_ms - *(int *)(iface + 0x78);
  }
  if (elapsed_ms < 1) {
    *out_rate = 0;
    LeaveCriticalSection(cs);
    return 0;
  }
  elapsed_ms = MulDiv(100000,*(int *)(iface + 0x44),elapsed_ms);
  *out_rate = elapsed_ms;
  LeaveCriticalSection(cs);
  return 0;
}


