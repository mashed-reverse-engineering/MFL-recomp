// cluster-entries: FUN_004acfd0
/* Installed cluster FUN_004acfd0 (tools/cluster_install.py from output/cluster_pilot9/FUN_004acfd0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall timeGetTime(void);
/* FUN_004acfd0 */

undefined4 FUN_004acfd0_n(char *param_1)

{
  DWORD DVar1;
  
  DVar1 = timeGetTime();
  *(DWORD *)(param_1 + 0x158) = DVar1 - *(int *)(param_1 + 0x158);
  return 0;
}

/* entry thunk at the original address of FUN_004acfd0: register inputs (ecx) -> natural C FUN_004acfd0_n; ret 0 */
__declspec(naked) void FUN_004acfd0(void)
{
    __asm {
        push ecx
        call FUN_004acfd0_n
        add esp, 4
        ret
    }
}
