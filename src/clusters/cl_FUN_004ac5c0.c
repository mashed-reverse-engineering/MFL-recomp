// cluster-entries: FUN_004ac5c0
/* Installed cluster FUN_004ac5c0 (tools/cluster_install.py from output/cluster_pilot9/FUN_004ac5c0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall DeleteCriticalSection(int);
/* FUN_004ac5c0 */

void FUN_004ac5c0_n(LPCRITICAL_SECTION param_1)

{
  DeleteCriticalSection(param_1);
  return;
}

/* entry thunk at the original address of FUN_004ac5c0: register inputs (ecx) -> natural C FUN_004ac5c0_n; ret 0 */
__declspec(naked) void FUN_004ac5c0(void)
{
    __asm {
        push ecx
        call FUN_004ac5c0_n
        add esp, 4
        ret
    }
}
