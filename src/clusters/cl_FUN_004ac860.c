// cluster-entries: FUN_004ac860
/* Installed cluster FUN_004ac860 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004ac860.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *);
typedef int (__stdcall *ComMethod1)(int *self);
typedef int (__fastcall *ThisMethod0)(int *self, int unused_edx);
/* FUN_004ac860 */

int *FUN_004ac860_n(int param_1)

{
  int *piVar1;
  
  EnterCriticalSection((LPCRITICAL_SECTION)(param_1 + 0x94));
  piVar1 = *(int **)(param_1 + 0x6c);
  if (piVar1 != (int *)0x0) {
    (*(ComMethod1 *)(*piVar1 + 4))(piVar1);
  }
  piVar1 = *(int **)(param_1 + 0x6c);
  LeaveCriticalSection((LPCRITICAL_SECTION)(param_1 + 0x94));
  return piVar1;
}

/* entry thunk at the original address of FUN_004ac860: register inputs (ecx) -> natural C FUN_004ac860_n; ret 0 */
__declspec(naked) void FUN_004ac860(void)
{
    __asm {
        push ecx
        call FUN_004ac860_n
        add esp, 4
        ret
    }
}
