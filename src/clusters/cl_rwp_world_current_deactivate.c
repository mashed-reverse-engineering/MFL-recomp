// cluster-entries: rwp_world_current_deactivate
/* Installed cluster rwp_world_current_deactivate (tools/cluster_install.py from output/cluster_pilot6/rwp_world_current_deactivate.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster rwp_world_current_deactivate: hands the argument and the object FUN_005543d0 derives from the current world to rwp_world_deactivate_object (deactivate); returns its result. */
extern void *g_rw_701c9c;
extern void *FUN_005543d0(void *world);
extern int rwp_world_deactivate_object(void *object, void *arg);

/* was: EAX = object to deactivate */
int rwp_world_current_deactivate_n(void *object)
{
    return rwp_world_deactivate_object(FUN_005543d0(g_rw_701c9c), object);
}

/* entry thunk at the original address of rwp_world_current_deactivate: register inputs (eax) -> natural C rwp_world_current_deactivate_n; ret 0 */
__declspec(naked) void rwp_world_current_deactivate(void)
{
    __asm {
        push eax
        call rwp_world_current_deactivate_n
        add esp, 4
        ret
    }
}
