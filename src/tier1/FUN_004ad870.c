#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall EnterCriticalSection(int);
extern __declspec(dllimport) int __stdcall LeaveCriticalSection(int);
/* FUN_004ad870 */

undefined4 __stdcall FUN_004ad870(char *self, undefined4 *out)

{
  if (out == (undefined4 *)0x0) {
    return 0x80004003;
  }
  EnterCriticalSection((LPCRITICAL_SECTION)(self + -100));
  *out = *(undefined4 *)(self + 0x40);
  LeaveCriticalSection((LPCRITICAL_SECTION)(self + -100));
  return 0;
}


