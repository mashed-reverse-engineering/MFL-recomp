// cluster-entries: debris_half_update_visible_both
/* Installed cluster debris_half_update_visible_both (tools/cluster_install.py from output/cluster_pilot10/debris_half_update_visible_both.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00424d40*/ extern undefined __cdecl debris_slot_list_update_visible(undefined4 p0);
/* debris_half_update_visible_both */

/* WARNING: Unknown calling convention */
/* "runs debris_slot_list_update_visible for 2 halves (stride 0x100)" */

void debris_half_update_visible_both_n(int in_eax)

{
  int iVar1;
  
  iVar1 = 2;
  do {
    debris_slot_list_update_visible(in_eax);
    in_eax = in_eax + 0x100;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}

/* entry thunk at the original address of debris_half_update_visible_both: register inputs (eax) -> natural C debris_half_update_visible_both_n; ret 0 */
__declspec(naked) void debris_half_update_visible_both(void)
{
    __asm {
        push eax
        call debris_half_update_visible_both_n
        add esp, 4
        ret
    }
}
