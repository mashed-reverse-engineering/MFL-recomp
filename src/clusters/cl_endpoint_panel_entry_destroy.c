// cluster-entries: endpoint_panel_entry_destroy
/* Installed cluster endpoint_panel_entry_destroy (tools/cluster_install.py from output/cluster_pilot10/endpoint_panel_entry_destroy.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/* endpoint_panel_entry_destroy */

/*  */

void endpoint_panel_entry_destroy_n(int in_eax)

{
  RpClumpDestroy(*(RpClump **)(in_eax + 0x150));
  return;
}

/* entry thunk at the original address of endpoint_panel_entry_destroy: register inputs (eax) -> natural C endpoint_panel_entry_destroy_n; ret 0 */
__declspec(naked) void endpoint_panel_entry_destroy(void)
{
    __asm {
        push eax
        call endpoint_panel_entry_destroy_n
        add esp, 4
        ret
    }
}
