// cluster-entries: kc_panel_entry_destroy
/* Installed cluster kc_panel_entry_destroy (tools/cluster_install.py from output/cluster_pilot6/kc_panel_entry_destroy.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster kc_panel_entry_destroy: destroys the clump of a key-config panel entry. */
extern void RpClumpDestroy(void *clump);

/* was: EAX = panel entry */
void kc_panel_entry_destroy_n(char *entry)
{
    RpClumpDestroy(*(void **)(entry + 0x100));
}

/* entry thunk at the original address of kc_panel_entry_destroy: register inputs (eax) -> natural C kc_panel_entry_destroy_n; ret 0 */
__declspec(naked) void kc_panel_entry_destroy(void)
{
    __asm {
        push eax
        call kc_panel_entry_destroy_n
        add esp, 4
        ret
    }
}
