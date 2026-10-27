#include "mfl_tier2.h"
/*00486710*/ extern int particles_destroy();
/*007519e4*/ extern undefined4 g_particles_7519e4;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00752184*/ extern char g_particles_752184[];


#define FX_SYSTEM_PARTICLES_OFFSET 0x10
#define FX_SYSTEM_STRIDE_DWORDS    0x7a
#define FX_SYSTEMS_END_ADDR        ((int)&g_particles_752184)


/* fx_systems_destroy */

/* Destroys the 4 particle/fx systems and frees their allocated buffers. */

void fx_systems_destroy(void)
{
  int *fx_system;
  
  fx_system = &g_particles_7519e4;
  do {
    /* Destroy particle system at offset 0x10 in the system buffer */
    particles_destroy(*fx_system + FX_SYSTEM_PARTICLES_OFFSET);
    /* Free system buffer via RenderWare rwfree callback (field_0x10c) */
    RwFree(*(undefined4 *)*fx_system);
    /* Advance to the next system (0x7a dwords = 0x1e8 bytes) */
    fx_system = fx_system + FX_SYSTEM_STRIDE_DWORDS;
  } while ((int)fx_system < FX_SYSTEMS_END_ADDR);
  return;
}
