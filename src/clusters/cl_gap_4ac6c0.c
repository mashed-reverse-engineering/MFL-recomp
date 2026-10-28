// cluster-entries: gap_4ac6c0
/* Installed cluster gap_4ac6c0 (tools/cluster_install.py from output/cluster_pilot_hb1/gap_4ac6c0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* gap_4ac6c0 */

typedef int (__stdcall *ComMethod3)(int *self, uint *arg1, uint *arg2);
typedef int (__fastcall *ThisMethod3)(int *self, int unused_edx, int *arg0, uint *arg1, uint *arg2);

/* Calls COM vtable[5] on stream to get two 64-bit values; returns 0x80040228 if pos_b is not past pos_a, else forwards to this->vtable[0x17] when self[6] is set (inferred). */
undefined4 gap_4ac6c0_n(int *self, int *stream, uint *pos_a, uint *pos_b)

{
  int hr;
  undefined4 result;
  
  hr = (*(ComMethod3 *)(*stream + 0x14))(stream,pos_a,pos_b);
  if (-1 < hr) {
    if (((int)pos_b[1] <= (int)pos_a[1]) &&
       (((int)pos_b[1] < (int)pos_a[1] || (*pos_b < *pos_a)))) {
      return 0x80040228;
    }
    if (self[6] != 0) {
      result = (*(ThisMethod3 *)(*self + 0x5c))(self,0,stream,pos_a,pos_b);
      return result;
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
