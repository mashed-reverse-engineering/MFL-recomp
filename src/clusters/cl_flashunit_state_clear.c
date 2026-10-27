// cluster-entries: flashunit_state_clear
/* Installed cluster flashunit_state_clear (tools/cluster_install.py from output/cluster_pilot10/flashunit_state_clear.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* flashunit_state_clear */

/* "clears +0xC..+0x14" */

void flashunit_state_clear_n(int in_eax)

{
  *(undefined4 *)(in_eax + 0xc) = 0;
  *(undefined4 *)(in_eax + 0x10) = 0;
  *(undefined4 *)(in_eax + 0x14) = 0;
  return;
}

/* entry thunk at the original address of flashunit_state_clear: register inputs (eax) -> natural C flashunit_state_clear_n; ret 0 */
__declspec(naked) void flashunit_state_clear(void)
{
    __asm {
        push eax
        push edx
        push eax
        call flashunit_state_clear_n
        add esp, 4
        pop edx
        pop eax
        ret
    }
}
