// cluster-entries: gap_4aede0
/* Installed cluster gap_4aede0 (tools/cluster_install.py from output/cluster_pilot_hb1/gap_4aede0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* gap_4aede0 */

typedef int (__stdcall *ComMethod2)(int *self, uint *arg);

int gap_4aede0_n(int param_1, uint *param_2)

{
  int *piVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  
  piVar1 = *(int **)(param_1 + 0x18);
  if (piVar1 == (int *)0x0) {
    return -0x7ffbfded;
  }
  iVar4 = (*(ComMethod2 *)(*piVar1 + 0xc))(piVar1,param_2);
  if (-1 < iVar4) {
    uVar2 = *(uint *)(param_1 + 0x20);
    uVar3 = *param_2;
    *param_2 = uVar3 - uVar2;
    param_2[1] = (param_2[1] - *(int *)(param_1 + 0x24)) - (uint)(uVar3 < uVar2);
    iVar4 = 0;
  }
  return iVar4;
}

/* entry thunk at the original address of gap_4aede0: register inputs (ecx,s0) -> natural C gap_4aede0_n; ret 4 */
__declspec(naked) void gap_4aede0(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call gap_4aede0_n
        add esp, 8
        ret 4
    }
}
