// cluster-entries: gap_4acfc0
/* Installed cluster gap_4acfc0 (tools/cluster_install.py from output/cluster_pilot_hb1/gap_4acfc0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* gap_4acfc0 */

typedef void (__fastcall *ThisMethod0)(int *self, int unused_edx);

undefined4 gap_4acfc0_n(int *param_1)

{
  (*(ThisMethod0 *)(*param_1 + 0xbc))(param_1,0);
  return 0;
}

/* entry thunk at the original address of gap_4acfc0: register inputs (ecx) -> natural C gap_4acfc0_n; ret 0 */
__declspec(naked) void gap_4acfc0(void)
{
    __asm {
        push ecx
        call gap_4acfc0_n
        add esp, 4
        ret
    }
}
