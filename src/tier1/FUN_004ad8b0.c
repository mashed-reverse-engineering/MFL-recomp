#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall EnterCriticalSection(int);
extern __declspec(dllimport) int __stdcall LeaveCriticalSection(int);
/* FUN_004ad8b0 */

undefined4 __stdcall FUN_004ad8b0(char *param_1, undefined4 *param_2)

{
  if (param_2 == (undefined4 *)0x0) {
    return 0x80004003;
  }
  EnterCriticalSection((LPCRITICAL_SECTION)(param_1 + -100));
  *param_2 = *(undefined4 *)(param_1 + 0x44);
  LeaveCriticalSection((LPCRITICAL_SECTION)(param_1 + -100));
  return 0;
}


