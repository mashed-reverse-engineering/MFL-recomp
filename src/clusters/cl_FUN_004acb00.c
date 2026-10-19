// cluster-entries: FUN_004acb00
/* Installed cluster FUN_004acb00 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004acb00.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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
/* FUN_004acb00 */

undefined4 FUN_004acb00_n(int *param_1)

{
  EnterCriticalSection((LPCRITICAL_SECTION)(param_1 + 0x25));
  param_1[0x1d] = 0;
  if (param_1[0x19] == 1) {
    param_1[0x19] = 0;
    (*(ThisMethod0 *)(*param_1 + 0x44))(param_1,0);
    timeEndPeriod(1);
  }
  LeaveCriticalSection((LPCRITICAL_SECTION)(param_1 + 0x25));
  return 0;
}

/* entry thunk at the original address of FUN_004acb00: register inputs (ecx) -> natural C FUN_004acb00_n; ret 0 */
__declspec(naked) void FUN_004acb00(void)
{
    __asm {
        push ecx
        call FUN_004acb00_n
        add esp, 4
        ret
    }
}
