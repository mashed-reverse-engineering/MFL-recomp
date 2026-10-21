#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall EnterCriticalSection(int);
extern __declspec(dllimport) int __stdcall LeaveCriticalSection(int);
/* FUN_004ad970 */
undefined4 __stdcall FUN_004ad970(int param_1, int *param_2)
{
  int lock;

  if (param_2 == 0) {
    return 0x80004003;
  }
  lock = param_1 - 100;
  EnterCriticalSection(lock);
  if (*(int *)(param_1 - 200) != 0 && 1 < *(int *)(param_1 + 0x44)) {
    *param_2 = (int)(*(__int64 *)(param_1 + 0x48) / (*(int *)(param_1 + 0x44) - 1));
    LeaveCriticalSection(lock);
    return 0;
  }
  *param_2 = 0;
  LeaveCriticalSection(lock);
  return 0;
}
