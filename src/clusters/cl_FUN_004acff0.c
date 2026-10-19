// cluster-entries: FUN_004acff0
/* Installed cluster FUN_004acff0 (tools/cluster_install.py from output/cluster_pilot9/FUN_004acff0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* FUN_004acff0 */

void FUN_004acff0_n(int in_ecx, undefined4 param_1, undefined4 param_2)

{
  
  *(undefined4 *)(in_ecx + 0x150) = param_1;
  *(undefined4 *)(in_ecx + 0x154) = param_2;
  return;
}

/* entry thunk at the original address of FUN_004acff0: register inputs (ecx,s0,s1) -> natural C FUN_004acff0_n; ret 8 */
__declspec(naked) void FUN_004acff0(void)
{
    __asm {
        push ecx
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push ecx
        call FUN_004acff0_n
        add esp, 12
        pop ecx
        ret 8
    }
}
