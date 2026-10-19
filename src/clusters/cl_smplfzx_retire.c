// cluster-entries: smplfzx_retire smplfzx_retire_object
/* Installed cluster smplfzx_retire (tools/cluster_install.py from output/cluster_pilot2/smplfzx_retire.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster smplfzx_retire: returns a smoke / flash effect object (SmplFzx) to its pool, looked up by its id. */

struct SmplFzxObject {
    char unknown_00[0xc];
    int id;                       /* 0x0c */
};

extern void *g_particles_71abf4;  /* list of the active SmplFzx objects */
extern void *g_smplfzx_71abf0;    /* pool of free SmplFzx objects */
extern const char g_smplfzx_retire_5e06e8[];

extern void debug_printf(const char *fmt, ...);
extern int pool_list_unlink(void *list, void *item);
extern void pool_push(void *pool, void *item);
extern struct SmplFzxObject *smplfzx_check_active(unsigned int id);

/* Moves an active object back to the free pool (reports a NULL object or an object that is not in the active list). */
void smplfzx_retire_n(struct SmplFzxObject *obj)
{
    if (obj == NULL) {
        debug_printf("ERROR: SmplFzx. Retiring NULL object!!\n");
        return;
    }
    debug_printf("SmplFzx : Retiring %d\n", obj->id);
    if (pool_list_unlink(g_particles_71abf4, obj) == 0) {
        debug_printf(g_smplfzx_retire_5e06e8, obj->id);
        return;
    }
    pool_push(g_smplfzx_71abf0, obj);
}

/* Retires the active object with the given id (the id is 16 bits). */
void smplfzx_retire_object(unsigned int id)
{
    struct SmplFzxObject *obj;

    debug_printf("SmplFzx Retiring : %d\n", (unsigned short)id);
    obj = smplfzx_check_active((unsigned short)id);
    smplfzx_retire_n(obj);
}

/* entry thunk at the original address of smplfzx_retire: register inputs (esi) -> natural C smplfzx_retire_n; ret 0 */
__declspec(naked) void smplfzx_retire(void)
{
    __asm {
        push esi
        call smplfzx_retire_n
        add esp, 4
        ret
    }
}
