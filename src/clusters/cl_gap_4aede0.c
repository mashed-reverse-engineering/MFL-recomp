// cluster-entries: gap_4aede0
/* Installed cluster gap_4aede0 (tools/cluster_install.py from output/cluster_pilot_hb1/gap_4aede0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* gap_4aede0 */

typedef int (__stdcall *ComMethod2)(int *self, uint *arg);

/* COM call (vtable slot 3) filling out_range, then subtracts the 64-bit base (obj+0x20/0x24) from it; returns HRESULT or 0 (inferred). */
int gap_4aede0_n(int obj, uint *out_range)

{
  int *com_obj;
  uint base_lo;
  uint old_lo;
  int hr;
  
  com_obj = *(int **)(obj + 0x18);
  if (com_obj == (int *)0x0) {
    return -0x7ffbfded;
  }
  hr = (*(ComMethod2 *)(*com_obj + 0xc))(com_obj,out_range);
  if (-1 < hr) {
    base_lo = *(uint *)(obj + 0x20);
    old_lo = *out_range;
    *out_range = old_lo - base_lo;
    out_range[1] = (out_range[1] - *(int *)(obj + 0x24)) - (uint)(old_lo < base_lo);
    hr = 0;
  }
  return hr;
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
