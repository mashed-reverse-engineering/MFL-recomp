// cluster-entries: FUN_004aef40
/* Installed cluster FUN_004aef40 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004aef40.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
typedef int (__stdcall *ComMethod4)(int *self, int a, int b, int c);
/* FUN_004aef40 */

undefined4 FUN_004aef40_n(char *in_ecx, int param_1, undefined4 param_2, undefined4 param_3)

{
  int *piVar1;
  undefined4 uVar2;
  
  piVar1 = *(int **)(in_ecx + 0x44);
  if (piVar1 == (int *)0x0) {
    return 0x80004001;
  }
  if (param_1 == 1) {
    uVar2 = (*(ComMethod4 *)(*piVar1 + 0xc))(piVar1,1,param_2,in_ecx + 0xc);
    return uVar2;
  }
  uVar2 = (*(ComMethod4 *)(*piVar1 + 0xc))(piVar1,param_1,param_2,param_3);
  return uVar2;
}

/* entry thunk at the original address of FUN_004aef40: register inputs (ecx,s0,s1,s2) -> natural C FUN_004aef40_n; ret 12 */
__declspec(naked) void FUN_004aef40(void)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push ecx
        call FUN_004aef40_n
        add esp, 16
        ret 12
    }
}
