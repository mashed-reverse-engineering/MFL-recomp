// cluster-entries: world_sector_list_add
/* Installed cluster world_sector_list_add (tools/cluster_install.py from output/cluster_pilot5/world_sector_list_add.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster world_sector_list_add: collects the world sectors the world render pass visits (counters + list), then calls the original render callback unless culling is on. */
extern void *g_world_sector_702aa0[];
extern int g_physics_7032a8;
extern int g_physics_7036c0;
extern int g_particles_7036c4;
extern void (*g_physics_702a98)(void *sector);

/* was: ECX = sector */
void world_sector_list_add_n(void *sector)
{
    g_world_sector_702aa0[g_physics_7032a8] = sector;
    g_physics_7032a8++;
    if (g_physics_7036c0 == 0 && g_particles_7036c4 == 0) {
        g_physics_702a98(sector);
    }
}

/* entry thunk at the original address of world_sector_list_add: register inputs (ecx) -> natural C world_sector_list_add_n; ret 0 */
__declspec(naked) void world_sector_list_add(void)
{
    __asm {
        push ecx
        call world_sector_list_add_n
        add esp, 4
        ret
    }
}
