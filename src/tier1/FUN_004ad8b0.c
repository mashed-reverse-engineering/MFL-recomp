#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall EnterCriticalSection(int);
extern __declspec(dllimport) int __stdcall LeaveCriticalSection(int);
/* FUN_004ad8b0 */

/* Getter (inferred: COM-style, HRESULT stdcall): copies the u32 at +0x44 of self to *out while holding the critical section at self-100 (returns 0x80004003 = E_POINTER if out is null). */
undefined4 __stdcall FUN_004ad8b0(char *self, undefined4 *out)

{
  if (out == (undefined4 *)0x0) {
    return 0x80004003;
  }
  EnterCriticalSection((LPCRITICAL_SECTION)(self + -100));
  *out = *(undefined4 *)(self + 0x44);
  LeaveCriticalSection((LPCRITICAL_SECTION)(self + -100));
  return 0;
}


