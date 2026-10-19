// cluster-entries: FUN_004ac930
/* Installed cluster FUN_004ac930 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004ac930.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *);
typedef int (__stdcall *ComMethod1)(int *self);
typedef int (__fastcall *ThisMethod0)(int *self, int unused_edx);
/* FUN_004ac930 */

void FUN_004ac930_n(int *param_1)

{
  EnterCriticalSection((LPCRITICAL_SECTION)(param_1 + 0x25));
  if (param_1[0x30] != 0) {
    param_1[0x30] = 0;
    (*(ThisMethod0 *)(*param_1 + 0x60))(param_1,0);
  }
  LeaveCriticalSection((LPCRITICAL_SECTION)(param_1 + 0x25));
  return;
}

/* entry thunk at the original address of FUN_004ac930: register inputs (ecx) -> natural C FUN_004ac930_n; ret 0 */
__declspec(naked) void FUN_004ac930(void)
{
    __asm {
        push ecx
        call FUN_004ac930_n
        add esp, 4
        ret
    }
}
