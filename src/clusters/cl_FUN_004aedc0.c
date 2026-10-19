// cluster-entries: FUN_004aedc0
/* Installed cluster FUN_004aedc0 (tools/cluster_install.py from output/cluster_pilot9/FUN_004aedc0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004b0f00*/ extern int __cdecl FUN_004b0f00(void);
/* FUN_004aedc0 */

void FUN_004aedc0_n(int param_1)

{
  if (param_1 != 0) {
    FUN_004b0f00();
    return;
  }
  FUN_004b0f00();
  return;
}

/* entry thunk at the original address of FUN_004aedc0: register inputs (ecx) -> natural C FUN_004aedc0_n; ret 0 */
__declspec(naked) void FUN_004aedc0(void)
{
    __asm {
        push eax
        push edx
        push ecx
        call FUN_004aedc0_n
        add esp, 4
        pop edx
        pop eax
        ret
    }
}
