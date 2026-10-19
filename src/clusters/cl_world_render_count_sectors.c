// cluster-entries: world_render_count_sectors
/* Installed cluster world_render_count_sectors (tools/cluster_install.py from output/cluster_pilot6/world_render_count_sectors.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster world_render_count_sectors: renders the world with the sector callback replaced by the counting callback, restores it and keeps the sector counters. */
extern void *g_physics_702a98;
extern int g_physics_7036b0;
extern int g_physics_702a94;
extern int g_physics_702a90;
extern int g_physics_7036b4;
extern void *RpWorldGetSectorRenderCallBack(void *world);
extern void *RpWorldSetSectorRenderCallBack(void *world, void *callback);
extern void *RpWorldRender(void *world);
extern void LAB_00495e30(void);

/* was: ESI = world */
void world_render_count_sectors_n(void *world)
{
    g_physics_702a98 = RpWorldGetSectorRenderCallBack(world);
    RpWorldSetSectorRenderCallBack(world, LAB_00495e30);
    RpWorldRender(world);
    RpWorldSetSectorRenderCallBack(world, g_physics_702a98);
    g_physics_702a90 = g_physics_7036b0;
    g_physics_7036b4 = g_physics_702a94;
}

/* entry thunk at the original address of world_render_count_sectors: register inputs (esi) -> natural C world_render_count_sectors_n; ret 0 */
__declspec(naked) void world_render_count_sectors(void)
{
    __asm {
        push esi
        call world_render_count_sectors_n
        add esp, 4
        ret
    }
}
