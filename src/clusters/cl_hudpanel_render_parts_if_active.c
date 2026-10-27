// cluster-entries: hudpanel_render_parts_if_active
/* Installed cluster hudpanel_render_parts_if_active (tools/cluster_install.py from output/cluster_pilot10/hudpanel_render_parts_if_active.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* hudpanel_render_parts_if_active */

/* "calls virtual render method (0x48) of up to 6 parts when active" */

void hudpanel_render_parts_if_active_n(int *in_eax)

{
  if (in_eax[0x14] != 0) {
    if (*in_eax != 0) {
      (**(code **)(in_eax[8] + 0x48))(in_eax[8]);
    }
    if (in_eax[1] != 0) {
      (**(code **)(in_eax[9] + 0x48))(in_eax[9]);
    }
    if (in_eax[2] != 0) {
      (**(code **)(in_eax[10] + 0x48))(in_eax[10]);
    }
    if (in_eax[3] != 0) {
      (**(code **)(in_eax[0xb] + 0x48))(in_eax[0xb]);
    }
    if (in_eax[4] != 0) {
      (**(code **)(in_eax[0xc] + 0x48))(in_eax[0xc]);
    }
    if (in_eax[5] != 0) {
      (**(code **)(in_eax[0xd] + 0x48))(in_eax[0xd]);
    }
  }
  return;
}

/* entry thunk at the original address of hudpanel_render_parts_if_active: register inputs (eax) -> natural C hudpanel_render_parts_if_active_n; ret 0 */
__declspec(naked) void hudpanel_render_parts_if_active(void)
{
    __asm {
        push eax
        call hudpanel_render_parts_if_active_n
        add esp, 4
        ret
    }
}
