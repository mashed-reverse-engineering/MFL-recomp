// cluster-entries: gap_4ac7f0
/* Installed cluster gap_4ac7f0 (tools/cluster_install.py from output/cluster_pilot_hb1/gap_4ac7f0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* gap_4ac7f0 */

typedef int (__fastcall *ThisMethod1)(int *self, int unused_edx, int arg);

undefined4 gap_4ac7f0_n(int *param_1, int param_2)

{
  if ((param_2 != 0) && (param_1[0x19] != 0)) {
    (*(ThisMethod1 *)(*param_1 + 0x38))(param_1,0,param_2);
    (*(ThisMethod1 *)(*param_1 + 0xac))(param_1,0,param_2);
    (*(ThisMethod1 *)(*param_1 + 0x3c))(param_1,0,param_2);
    return 0;
  }
  return 1;
}

/* entry thunk at the original address of gap_4ac7f0: register inputs (ecx,s0) -> natural C gap_4ac7f0_n; ret 4 */
__declspec(naked) void gap_4ac7f0(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call gap_4ac7f0_n
        add esp, 8
        ret 4
    }
}
