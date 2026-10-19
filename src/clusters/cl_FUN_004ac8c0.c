// cluster-entries: FUN_004ac8c0
/* Installed cluster FUN_004ac8c0 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004ac8c0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *);
typedef int (__stdcall *ComMethod1)(int *self);
typedef int (__fastcall *ThisMethod0)(int *self, int unused_edx);
/* FUN_004ac8c0 */

undefined4 FUN_004ac8c0_n(int param_1)

{
  int *piVar1;
  
  EnterCriticalSection((LPCRITICAL_SECTION)(param_1 + 0x94));
  piVar1 = *(int **)(param_1 + 0x6c);
  if (piVar1 != (int *)0x0) {
    (*(ComMethod1 *)(*piVar1 + 8))(piVar1);
    *(undefined4 *)(param_1 + 0x6c) = 0;
  }
  LeaveCriticalSection((LPCRITICAL_SECTION)(param_1 + 0x94));
  return 0;
}

/* entry thunk at the original address of FUN_004ac8c0: register inputs (ecx) -> natural C FUN_004ac8c0_n; ret 0 */
__declspec(naked) void FUN_004ac8c0(void)
{
    __asm {
        push ecx
        call FUN_004ac8c0_n
        add esp, 4
        ret
    }
}
