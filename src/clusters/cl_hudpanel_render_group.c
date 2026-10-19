// cluster-entries: hudpanel_render_group
/* Installed cluster hudpanel_render_group (tools/cluster_install.py from output/cluster_pilot10/hudpanel_render_group.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* hudpanel_render_group */

/* WARNING: Unknown calling convention */
/* "calls the render callback of the four clumps of the group" */

void hudpanel_render_group_n(int *in_eax)

{
  (**(code **)(in_eax[3] + 0x48))(in_eax[3]);
  (**(code **)(in_eax[1] + 0x48))(in_eax[1]);
  (**(code **)(*in_eax + 0x48))(*in_eax);
  (**(code **)(in_eax[2] + 0x48))(in_eax[2]);
  return;
}

/* entry thunk at the original address of hudpanel_render_group: register inputs (eax) -> natural C hudpanel_render_group_n; ret 0 */
__declspec(naked) void hudpanel_render_group(void)
{
    __asm {
        push eax
        call hudpanel_render_group_n
        add esp, 4
        ret
    }
}
