// cluster-entries: FUN_004afb80
/* Installed cluster FUN_004afb80 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004afb80.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
typedef int (__stdcall *ComMethod1)(int *self);
/* FUN_004afb80 */

int FUN_004afb80_n(int param_1)

{
  int *piVar1;
  int iVar2;
  
  piVar1 = *(int **)(param_1 + 0x9c);
  if (piVar1 != (int *)0x0) {
    iVar2 = (*(ComMethod1 *)(*piVar1 + 0x18))(piVar1);
    if (iVar2 < 0) {
      return iVar2;
    }
    (*(ComMethod1 *)(**(int **)(param_1 + 0x9c) + 8))(*(int **)(param_1 + 0x9c));
    *(undefined4 *)(param_1 + 0x9c) = 0;
  }
  return 0;
}

/* entry thunk at the original address of FUN_004afb80: register inputs (ecx) -> natural C FUN_004afb80_n; ret 0 */
__declspec(naked) void FUN_004afb80(void)
{
    __asm {
        push ecx
        call FUN_004afb80_n
        add esp, 4
        ret
    }
}
