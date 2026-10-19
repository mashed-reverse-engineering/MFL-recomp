// cluster-entries: FUN_004ac830
/* Installed cluster FUN_004ac830 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004ac830.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *);
/* FUN_004ac830 */

bool FUN_004ac830_n(int param_1)

{
  int iVar1;
  
  EnterCriticalSection((LPCRITICAL_SECTION)(param_1 + 0x94));
  iVar1 = *(int *)(param_1 + 0x6c);
  LeaveCriticalSection((LPCRITICAL_SECTION)(param_1 + 0x94));
  return iVar1 != 0;
}

/* entry thunk at the original address of FUN_004ac830: register inputs (ecx) -> natural C FUN_004ac830_n; ret 0 */
__declspec(naked) void FUN_004ac830(void)
{
    __asm {
        push ecx
        call FUN_004ac830_n
        add esp, 4
        ret
    }
}
