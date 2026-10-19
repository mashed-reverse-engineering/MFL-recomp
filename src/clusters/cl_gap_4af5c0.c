// cluster-entries: gap_4af5c0
/* Installed cluster gap_4af5c0 (tools/cluster_install.py from output/cluster_pilot_hb1/gap_4af5c0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
typedef int (__stdcall *ComMethod2p)(int *self, int *out);
/* gap_4af5c0 */

/* "0x80040208 when the value the object reports equals the one stored at +0x1c, else 0" */

int gap_4af5c0_n(int *in_ecx, int *param_1)

{
  int reported;
  
  (*(ComMethod2p *)(*param_1 + 0x24))(param_1,&reported);
  return (reported == in_ecx[7]) ? 0x80040208 : 0;
}

/* entry thunk at the original address of gap_4af5c0: register inputs (ecx,s0) -> natural C gap_4af5c0_n; ret 4 */
__declspec(naked) void gap_4af5c0(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call gap_4af5c0_n
        add esp, 8
        ret 4
    }
}
