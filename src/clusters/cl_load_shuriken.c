// cluster-entries: load_shuriken
/* Installed cluster load_shuriken (tools/cluster_install.py from output/cluster_pilot6/load_shuriken.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster load_shuriken: finds the record of the Shuriken model table (0x84 bytes per record, key at +0x80) whose key is the argument; 0 when there is none. */
extern int g_load_shuriken_607618;
extern int g_load_shuriken_604830[];
extern char s_Shuriken_006047b0[];

/* was: ESI = key */
char *load_shuriken_n(int key)
{
    int i;

    for (i = 0; i < g_load_shuriken_607618; i++) {
        if (*(int *)((char *)g_load_shuriken_604830 + i * 0x84) == key) {
            return s_Shuriken_006047b0 + i * 0x84;
        }
    }
    return 0;
}

/* entry thunk at the original address of load_shuriken: register inputs (esi) -> natural C load_shuriken_n; ret 0 */
__declspec(naked) void load_shuriken(void)
{
    __asm {
        push esi
        call load_shuriken_n
        add esp, 4
        ret
    }
}
