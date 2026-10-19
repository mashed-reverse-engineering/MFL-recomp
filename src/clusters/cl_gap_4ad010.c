// cluster-entries: gap_4ad010
/* Installed cluster gap_4ad010 (tools/cluster_install.py from output/cluster_pilot_hb1/gap_4ad010.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* gap_4ad010 */

/* "accumulates two clamped samples (value / 10000) into running sums and sums of squares; the sample count is at +0x124" */

void gap_4ad010_n(char *in_ecx, int param_1, int param_2)

{
  int count;
  int a;
  int b;
  
  a = param_1 / 10000;
  if ((1000 < a) || (a < -1000)) {
    if (*(int *)(in_ecx + 0x124) < 2) {
      a = 0;
    }
    else {
      a = (a < 1) ? -1000 : 1000;
    }
  }
  count = *(int *)(in_ecx + 0x124);
  if (1 < count) {
    *(__int64 *)(in_ecx + 0x128) = *(__int64 *)(in_ecx + 0x128) + a;
    *(__int64 *)(in_ecx + 0x130) = *(__int64 *)(in_ecx + 0x130) + a * a;
  }
  if (2 < count) {
    b = param_2 / 10000;
    if ((1000 < b) || (b < 0)) {
      b = 1000;
    }
    *(__int64 *)(in_ecx + 0x140) = *(__int64 *)(in_ecx + 0x140) + b * b;
    *(__int64 *)(in_ecx + 0x148) = *(__int64 *)(in_ecx + 0x148) + b;
  }
  *(int *)(in_ecx + 0x124) = count + 1;
  return;
}

/* entry thunk at the original address of gap_4ad010: register inputs (ecx,s0,s1) -> natural C gap_4ad010_n; ret 8 */
__declspec(naked) void gap_4ad010(void)
{
    __asm {
        push ecx
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push ecx
        call gap_4ad010_n
        add esp, 12
        pop ecx
        ret 8
    }
}
