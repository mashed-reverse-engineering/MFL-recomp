// cluster-entries: hudpanel_slot_clear
/* Installed cluster hudpanel_slot_clear (tools/cluster_install.py from output/cluster_pilot10/hudpanel_slot_clear.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004162f0*/ extern int trail_set_param();
/*00472bb0*/ extern int sfx_voice_is_active();
/* hudpanel_slot_clear */

/* "clears the slot state words and the trail record of its player" */

void hudpanel_slot_clear_n(int in_eax)

{
  *(undefined4 *)(in_eax + 0x44) = 0;
  *(undefined4 *)(in_eax + 0x48) = 0;
  *(undefined4 *)(in_eax + 0x58) = 0;
  *(undefined4 *)(in_eax + 0x5c) = 0;
  *(undefined4 *)(in_eax + 0x50) = 0;
  *(undefined4 *)(in_eax + 0x54) = 0;
  trail_set_param(*(undefined4 *)(in_eax + 0x4c),2,0);
  sfx_voice_is_active(0x14,0);
  sfx_voice_is_active(0x14,1);
  sfx_voice_is_active(0x14,2);
  sfx_voice_is_active(0x14,3);
  return;
}

/* entry thunk at the original address of hudpanel_slot_clear: register inputs (eax) -> natural C hudpanel_slot_clear_n; ret 0 */
__declspec(naked) void hudpanel_slot_clear(void)
{
    __asm {
        push eax
        call hudpanel_slot_clear_n
        add esp, 4
        ret
    }
}
