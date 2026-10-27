// cluster-entries: weapon_fx_slot_clear_c
/* Installed cluster weapon_fx_slot_clear_c (tools/cluster_install.py from output/cluster_pilot10/weapon_fx_slot_clear_c.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/* weapon_fx_slot_clear_c */

/* "default scale 1.0, counters 0, clump flags cleared" */

void weapon_fx_slot_clear_c_n(undefined4 *in_eax)

{
  *in_eax = 0x3f800000;
  in_eax[2] = 0;
  in_eax[1] = 0;
  clump_set_flag_on_atomics((RpClump *)in_eax[3], 0x0);
  return;
}

/* entry thunk at the original address of weapon_fx_slot_clear_c: register inputs (eax) -> natural C weapon_fx_slot_clear_c_n; ret 0 */
__declspec(naked) void weapon_fx_slot_clear_c(void)
{
    __asm {
        push eax
        call weapon_fx_slot_clear_c_n
        add esp, 4
        ret
    }
}
