// cluster-entries: FUN_004aca50
/* Installed cluster FUN_004aca50 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004aca50.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *);
extern __declspec(dllimport) int __stdcall SetEvent(int);
extern __declspec(dllimport) int __stdcall ResetEvent(int);
extern __declspec(dllimport) int __stdcall timeBeginPeriod(int);
extern __declspec(dllimport) int __stdcall timeEndPeriod(int);
typedef int (__stdcall *ComMethod1)(int *self);
typedef int (__fastcall *ThisMethod0)(int *self, int unused_edx);
typedef int (__fastcall *ThisMethod1)(int *self, int unused_edx, int arg);
/* FUN_004aca50 */

int FUN_004aca50_n(int *param_1)

{
  LPCRITICAL_SECTION lpCriticalSection;
  int uVar1;
  int iVar2;
  
  lpCriticalSection = (LPCRITICAL_SECTION)(param_1 + 0x25);
  EnterCriticalSection(lpCriticalSection);
  if (param_1[0x19] != 1) {
    param_1[0x19] = 1;
    timeBeginPeriod(1);
    (*(ThisMethod0 *)(*param_1 + 0x40))(param_1,0);
    if (param_1[0x1b] == 0) {
      uVar1 = (*(ThisMethod0 *)(*param_1 + 0x60))(param_1,0);
      LeaveCriticalSection(lpCriticalSection);
      return uVar1;
    }
    iVar2 = (*(ThisMethod1 *)(*param_1 + 0x54))(param_1,0,param_1[0x1b]);
    if (iVar2 == 0) {
      SetEvent(param_1[0x15]);
    }
  }
  LeaveCriticalSection(lpCriticalSection);
  return 0;
}

/* entry thunk at the original address of FUN_004aca50: register inputs (ecx) -> natural C FUN_004aca50_n; ret 0 */
__declspec(naked) void FUN_004aca50(void)
{
    __asm {
        push ecx
        call FUN_004aca50_n
        add esp, 4
        ret
    }
}
