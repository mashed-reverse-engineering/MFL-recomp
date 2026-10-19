// cluster-entries: FUN_004acb70
/* Installed cluster FUN_004acb70 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004acb70.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *);
/* FUN_004acb70 */

void FUN_004acb70_n(int in_ecx, undefined4 param_1)

{
  EnterCriticalSection((LPCRITICAL_SECTION)(in_ecx + 0x94));
  *(undefined4 *)(in_ecx + 0xb0) = param_1;
  LeaveCriticalSection((LPCRITICAL_SECTION)(in_ecx + 0x94));
  return;
}

/* entry thunk at the original address of FUN_004acb70: register inputs (ecx,s0) -> natural C FUN_004acb70_n; ret 4 */
__declspec(naked) void FUN_004acb70(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call FUN_004acb70_n
        add esp, 8
        ret 4
    }
}
