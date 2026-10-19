// cluster-entries: gap_4adfb0
/* Installed cluster gap_4adfb0 (tools/cluster_install.py from output/cluster_pilot_hb1/gap_4adfb0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall SetEvent(int);
extern __declspec(dllimport) int __stdcall ResetEvent(int);
/* gap_4adfb0 */

undefined4 gap_4adfb0_n(int param_1, int param_2)

{
  if (param_2 == 1) {
    ResetEvent(*(int *)(param_1 + 0x58));
    return 0;
  }
  SetEvent(*(int *)(param_1 + 0x58));
  return 0;
}

/* entry thunk at the original address of gap_4adfb0: register inputs (ecx,s0) -> natural C gap_4adfb0_n; ret 4 */
__declspec(naked) void gap_4adfb0(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call gap_4adfb0_n
        add esp, 8
        ret 4
    }
}
