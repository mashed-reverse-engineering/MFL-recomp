// cluster-entries: FUN_004aca30
/* Installed cluster FUN_004aca30 (tools/cluster_install.py from output/cluster_pilot9/FUN_004aca30.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall timeKillEvent(int);
/* FUN_004aca30 */

void FUN_004aca30_n(char *param_1)

{
  if (*(UINT *)(param_1 + 0xc0) != 0) {
    timeKillEvent(*(UINT *)(param_1 + 0xc0));
    *(undefined4 *)(param_1 + 0xc0) = 0;
  }
  return;
}

/* entry thunk at the original address of FUN_004aca30: register inputs (ecx) -> natural C FUN_004aca30_n; ret 0 */
__declspec(naked) void FUN_004aca30(void)
{
    __asm {
        push ecx
        call FUN_004aca30_n
        add esp, 4
        ret
    }
}
