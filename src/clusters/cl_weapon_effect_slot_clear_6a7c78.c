// cluster-entries: weapon_effect_slot_clear_6a7c78
/* Installed cluster weapon_effect_slot_clear_6a7c78 (tools/cluster_install.py from output/cluster_pilot10/weapon_effect_slot_clear_6a7c78.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* weapon_effect_slot_clear_6a7c78 */

/* WARNING: Unknown calling convention */
/* "zero fields, +0x5C=-1.0" */

void weapon_effect_slot_clear_6a7c78_n(undefined4 *in_eax)

{
  *in_eax = 0;
  in_eax[0x18] = 0;
  in_eax[0x17] = 0xbf800000;
  in_eax[0x16] = 0;
  in_eax[1] = 0;
  return;
}

/* entry thunk at the original address of weapon_effect_slot_clear_6a7c78: register inputs (eax) -> natural C weapon_effect_slot_clear_6a7c78_n; ret 0 */
__declspec(naked) void weapon_effect_slot_clear_6a7c78(void)
{
    __asm {
        push eax
        push edx
        push eax
        call weapon_effect_slot_clear_6a7c78_n
        add esp, 4
        pop edx
        pop eax
        ret
    }
}
