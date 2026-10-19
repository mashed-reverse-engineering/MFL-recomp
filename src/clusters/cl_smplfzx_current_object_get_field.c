// cluster-entries: smplfzx_current_object_get_field smplfzx_object_get_position
/* Installed cluster smplfzx_current_object_get_field (tools/cluster_install.py from output/cluster_pilot5/smplfzx_current_object_get_field.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*004b322b*/ extern /* RET? */ undefined exit();
/*00496ea0*/ extern undefined __cdecl smplfzx_get_object(undefined4 p0);
/* DECLS-END */
/* Cluster smplfzx_current_object_get_field: first field of a smplfzx object record (20 byte records behind +0xc of the object list). */
typedef struct SmplList { char unknown_00[0xc]; char *records; } SmplList;
extern SmplList *g_particles_71abf4;

/* was: ESI = index */
char *smplfzx_current_object_get_field_n(int index)
{
    if (smplfzx_get_object(index) == 0) {
        exit(-11);
    }
    return **(char ***)(g_particles_71abf4->records + index * 20);
}

int smplfzx_object_get_position(int index, int *out)
{
    int *pos;

    if (smplfzx_get_object(index) == 0) {
        return 0;
    }
    pos = *(int **)(smplfzx_current_object_get_field_n(index) + 0x60);
    out[0] = pos[0];
    out[1] = pos[1];
    out[2] = pos[2];
    return 1;
}

/* entry thunk at the original address of smplfzx_current_object_get_field: register inputs (esi) -> natural C smplfzx_current_object_get_field_n; ret 0 */
__declspec(naked) void smplfzx_current_object_get_field(void)
{
    __asm {
        push esi
        call smplfzx_current_object_get_field_n
        add esp, 4
        ret
    }
}
