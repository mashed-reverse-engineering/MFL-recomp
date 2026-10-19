// cluster-entries: FUN_004ad120
/* Installed cluster FUN_004ad120 (tools/cluster_install.py from output/cluster_pilot9/FUN_004ad120.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall Sleep(int);
/* FUN_004ad120 */

void FUN_004ad120_n(char *param_1)

{
  if (0 < *(int *)(param_1 + 0xf0)) {
    Sleep(*(int *)(param_1 + 0xf0) / 10000);
    return;
  }
  Sleep(0);
  return;
}

/* entry thunk at the original address of FUN_004ad120: register inputs (ecx) -> natural C FUN_004ad120_n; ret 0 */
__declspec(naked) void FUN_004ad120(void)
{
    __asm {
        push ecx
        call FUN_004ad120_n
        add esp, 4
        ret
    }
}
