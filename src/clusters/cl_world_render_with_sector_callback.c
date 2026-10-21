// cluster-entries: world_render_with_sector_callback
/* Installed cluster world_render_with_sector_callback (tools/cluster_install.py from output/cluster_pilot_uc3/world_render_with_sector_callback.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004f5960*/ extern void *FUN_004f5960(void *world);      /* RpWorldGetSectorRenderCallBack */
/*004f5940*/ extern void *RpWorldSetSectorRenderCallBack(void *world, void *callback);
/*004f5780*/ extern void *RpWorldRender(void *world);
/*005543d0*/ extern int FUN_005543d0(void *stream);       /* first field of the debug flag chunk */
/*0042d9b0*/ extern void rw_stream_chunk_copy(void *stream, void *dst);
/*0042d9a0*/ extern void store_u32(void *stream, unsigned int value);
/*004c6310*/ extern void FUN_004c6310(void *item, unsigned int *colour);
/*00495ec0*/ extern void LAB_00495ec0(void);
/*00702a98*/ extern void *g_physics_702a98;
/*007036b8*/ extern int g_particle_system_7036b8;
/*007036cc*/ extern int g_particle_system_7036cc;
/*007032b0*/ extern void *g_particle_system_7032b0[];
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0068b8f8*/ extern char g_race_phase_arg[];

/* Renders `world` with the sector callback replaced by LAB_00495ec0 (it collects the sectors into g_particle_system_7032b0, count 7036b8), restores the callback,
   then reads the debug flag chunk; with the flag set every collected sector is drawn (FUN_004c6310, colour 0x400000ff) with the z test off.  Was: EAX = world. */
void world_render_with_sector_callback_n(void *world)
{
  int i;
  unsigned int colour;

  g_particle_system_7036b8 = 0;
  g_physics_702a98 = FUN_004f5960(world);
  RpWorldSetSectorRenderCallBack(world, LAB_00495ec0);
  RpWorldRender(world);
  RpWorldSetSectorRenderCallBack(world, g_physics_702a98);
  if (FUN_005543d0(g_race_phase_arg) == 2) {
    rw_stream_chunk_copy(g_race_phase_arg, &g_particle_system_7036cc);
    store_u32(g_race_phase_arg, 0);
  }
  if (g_particle_system_7036cc != 0) {
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)0);
    for (i = 0; i < g_particle_system_7036b8; i++) {
      colour = 0x400000ff;
      FUN_004c6310(g_particle_system_7032b0[i], &colour);
    }
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)1);
  }
}

/* entry thunk at the original address of world_render_with_sector_callback: register inputs (eax) -> natural C world_render_with_sector_callback_n; ret 0 */
__declspec(naked) void world_render_with_sector_callback(void)
{
    __asm {
        push eax
        call world_render_with_sector_callback_n
        add esp, 4
        ret
    }
}
