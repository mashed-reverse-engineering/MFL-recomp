// cluster-entries: FUN_004ac730
/* Installed cluster FUN_004ac730 (tools/cluster_install.py from output/cluster_pilot9/FUN_004ac730.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* FUN_004ac730 */

void FUN_004ac730_n(char *param_1)

{
  *(undefined4 *)(param_1 + 0x68) = 0;
  return;
}

/* entry thunk at the original address of FUN_004ac730: register inputs (ecx) -> natural C FUN_004ac730_n; ret 0 */
__declspec(naked) void FUN_004ac730(void)
{
    __asm {
        push eax
        push ecx
        push edx
        push ecx
        call FUN_004ac730_n
        add esp, 4
        pop edx
        pop ecx
        pop eax
        ret
    }
}
