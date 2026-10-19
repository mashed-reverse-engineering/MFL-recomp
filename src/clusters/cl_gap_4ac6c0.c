// cluster-entries: gap_4ac6c0
/* Installed cluster gap_4ac6c0 (tools/cluster_install.py from output/cluster_pilot_hb1/gap_4ac6c0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* gap_4ac6c0 */

typedef int (__stdcall *ComMethod3)(int *self, uint *arg1, uint *arg2);
typedef int (__fastcall *ThisMethod3)(int *self, int unused_edx, int *arg0, uint *arg1, uint *arg2);

undefined4 gap_4ac6c0_n(int *param_1, int *param_2, uint *param_3, uint *param_4)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = (*(ComMethod3 *)(*param_2 + 0x14))(param_2,param_3,param_4);
  if (-1 < iVar1) {
    if (((int)param_4[1] <= (int)param_3[1]) &&
       (((int)param_4[1] < (int)param_3[1] || (*param_4 < *param_3)))) {
      return 0x80040228;
    }
    if (param_1[6] != 0) {
      uVar2 = (*(ThisMethod3 *)(*param_1 + 0x5c))(param_1,0,param_2,param_3,param_4);
      return uVar2;
    }
  }
  return 0;
}

/* entry thunk at the original address of gap_4ac6c0: register inputs (ecx,s0,s1,s2) -> natural C gap_4ac6c0_n; ret 12 */
__declspec(naked) void gap_4ac6c0(void)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push ecx
        call gap_4ac6c0_n
        add esp, 16
        ret 12
    }
}
