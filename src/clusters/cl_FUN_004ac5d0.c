// cluster-entries: FUN_004ac5d0
/* Installed cluster FUN_004ac5d0 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004ac5d0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *);
/* FUN_004ac5d0 */

void FUN_004ac5d0_n(undefined4 *in_ecx, LPCRITICAL_SECTION param_1)

{
  *in_ecx = param_1;
  EnterCriticalSection(param_1);
  return;
}

/* entry thunk at the original address of FUN_004ac5d0: register inputs (ecx,s0) -> natural C FUN_004ac5d0_n; ret 4 */
__declspec(naked) void FUN_004ac5d0(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call FUN_004ac5d0_n
        add esp, 8
        ret 4
    }
}
