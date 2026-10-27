// cluster-entries: hudpanel_slot_reset
/* Installed cluster hudpanel_slot_reset (tools/cluster_install.py from output/cluster_pilot10/hudpanel_slot_reset.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00603e80*/ extern undefined g_hudpanel_603e80;
/*00603e84*/ extern undefined g_hudpanel_603e84;
/* hudpanel_slot_reset */

/* "copies the default position from the table at 0x603E80 and clears the slot state" */

void hudpanel_slot_reset_n(undefined4 *in_eax)

{
  *in_eax = *(undefined4 *)((char *)&g_hudpanel_603e80 + in_eax[0x14] * 8);
  in_eax[1] = *(undefined4 *)((char *)&g_hudpanel_603e84 + in_eax[0x14] * 8);
  in_eax[0xd] = 0;
  in_eax[0x19] = 0xffffffff;
  in_eax[0xe] = 0;
  in_eax[0xf] = 0;
  in_eax[0x10] = 0;
  in_eax[0x11] = 0;
  in_eax[0x13] = 0;
  return;
}

/* entry thunk at the original address of hudpanel_slot_reset: register inputs (eax) -> natural C hudpanel_slot_reset_n; ret 0 */
__declspec(naked) void hudpanel_slot_reset(void)
{
    __asm {
        push eax
        push eax
        call hudpanel_slot_reset_n
        add esp, 4
        pop eax
        ret
    }
}
