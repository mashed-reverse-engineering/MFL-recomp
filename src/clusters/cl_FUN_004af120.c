// cluster-entries: FUN_004af120
/* Installed cluster FUN_004af120 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004af120.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern char PTR_LAB_005e2484[];
/* FUN_004af120 */

typedef int (__stdcall *ComMethod1)(int *self);

void FUN_004af120_n(undefined4 *param_1)

{
  *param_1 = &PTR_LAB_005e2484;
  (*(ComMethod1 *)(*(int *)(param_1[2] + 0xc) + 8))((int *)(param_1[2] + 0xc));
  return;
}

/* entry thunk at the original address of FUN_004af120: register inputs (ecx) -> natural C FUN_004af120_n; ret 0 */
__declspec(naked) void FUN_004af120(void)
{
    __asm {
        push ecx
        call FUN_004af120_n
        add esp, 4
        ret
    }
}
