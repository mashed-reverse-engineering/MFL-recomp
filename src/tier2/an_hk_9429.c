#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall EnterCriticalSection(int);
extern __declspec(dllimport) int __stdcall LeaveCriticalSection(int);
extern __declspec(dllimport) int __stdcall MulDiv(int, int, int);
extern __declspec(dllimport) int __stdcall timeGetTime(void);
/* FUN_004ad8f0 */

undefined4 __stdcall FUN_004ad8f0(int param_1, int *param_2)

{
  LPCRITICAL_SECTION lpCriticalSection;
  DWORD DVar1;
  int iVar2;
  
  if (param_2 == (int *)0x0) {
    return 0x80004003;
  }
  lpCriticalSection = (LPCRITICAL_SECTION)(param_1 + -100);
  EnterCriticalSection(lpCriticalSection);
  if (*(int *)(param_1 + -0x7c) == 0) {
    iVar2 = *(int *)(param_1 + 0x78);
  }
  else {
    DVar1 = timeGetTime();
    iVar2 = DVar1 - *(int *)(param_1 + 0x78);
  }
  if (iVar2 < 1) {
    *param_2 = 0;
    LeaveCriticalSection(lpCriticalSection);
    return 0;
  }
  iVar2 = MulDiv(100000,*(int *)(param_1 + 0x44),iVar2);
  *param_2 = iVar2;
  LeaveCriticalSection(lpCriticalSection);
  return 0;
}


