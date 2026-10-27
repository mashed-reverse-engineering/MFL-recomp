#include "mfl_tier2.h"
/*004d5bf0*/ extern RwBool RwTextureDestroy(struct RwTexture *p0);
/*0076df24*/ extern undefined4 g_particles_76df24;


/* fx_texture_destroy */

/* "RwTextureDestroy(g_particles_76df24)" */

void fx_texture_destroy(void)
{
  RwTextureDestroy(g_particles_76df24);
  return;
}
