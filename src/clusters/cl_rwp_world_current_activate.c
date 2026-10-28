// cluster-entries: rwp_world_current_activate
/* Installed cluster rwp_world_current_activate (tools/cluster_install.py from output/cluster_pilot10/rwp_world_current_activate.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005543d0*/ extern int rw_first_field(int * p0);
/*0056a210*/ extern void rwp_world_activate_object(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*00701c9c*/ extern undefined4 g_rw_701c9c;
/* rwp_world_current_activate */

/* "rwp_world_activate_object on the current rwp world" */

void rwp_world_current_activate_n(undefined4 in_eax)

{
  undefined4 uVar1;
  
  uVar1 = rw_first_field(g_rw_701c9c);
  rwp_world_activate_object(uVar1, in_eax, 0xffffffff, 7);
  return;
}

/* entry thunk at the original address of rwp_world_current_activate: register inputs (eax) -> natural C rwp_world_current_activate_n; ret 0 */
__declspec(naked) void rwp_world_current_activate(void)
{
    __asm {
        push eax
        call rwp_world_current_activate_n
        add esp, 4
        ret
    }
}
