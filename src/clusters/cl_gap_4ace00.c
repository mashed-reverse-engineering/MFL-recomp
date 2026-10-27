// cluster-entries: gap_4ace00
/* Installed cluster gap_4ace00 (tools/cluster_install.py from output/cluster_pilot_hb1/gap_4ace00.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004ac6b0*/ extern int __stdcall FUN_004ac6b0(int a1);
/* gap_4ace00 */

typedef int (__fastcall *ThisMethod1)(int self, int unused_edx, int arg);

void gap_4ace00_n(int this_ptr, int arg)

{
  int hr;
  int self;

  self = *(int *)(this_ptr + 0xd8);
  hr = (*(ThisMethod1 *)(*(int *)self + 0x94))(self,0,arg);
  if (-1 < hr) {
    FUN_004ac6b0(arg);
  }
  return;
}

/* entry thunk at the original address of gap_4ace00: register inputs (ecx,s0) -> natural C gap_4ace00_n; ret 4 */
__declspec(naked) void gap_4ace00(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call gap_4ace00_n
        add esp, 8
        ret 4
    }
}
