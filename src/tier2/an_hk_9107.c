#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall EnterCriticalSection(int);
extern __declspec(dllimport) int __stdcall LeaveCriticalSection(int);
/* FUN_004afe60 */

undefined4 __stdcall FUN_004afe60(int param_1)

{
  int lpCriticalSection;
  
  lpCriticalSection = *(int *)(param_1 + 0x14);
  EnterCriticalSection(lpCriticalSection);
  *(undefined1 *)(param_1 + 0x95) = 0;
  *(undefined1 *)(param_1 + 0x18) = 0;
  LeaveCriticalSection(lpCriticalSection);
  return 0;
}


