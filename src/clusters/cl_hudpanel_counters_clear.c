// cluster-entries: hudpanel_counters_clear
/* Installed cluster hudpanel_counters_clear (tools/cluster_install.py from output/cluster_pilot10/hudpanel_counters_clear.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* hudpanel_counters_clear */

/* "clears the two counters at +0x4C / +0x50" */

void hudpanel_counters_clear_n(int in_eax)

{
  *(undefined4 *)(in_eax + 0x4c) = 0;
  *(undefined4 *)(in_eax + 0x50) = 0;
  return;
}

/* entry thunk at the original address of hudpanel_counters_clear: register inputs (eax) -> natural C hudpanel_counters_clear_n; ret 0 */
__declspec(naked) void hudpanel_counters_clear(void)
{
    __asm {
        push eax
        push edx
        push eax
        call hudpanel_counters_clear_n
        add esp, 4
        pop edx
        pop eax
        ret
    }
}
