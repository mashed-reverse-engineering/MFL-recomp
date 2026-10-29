/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef int (__cdecl *Fn3)(void *, void *, void *);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
#define ENG(off, T) (*(T *)(RwEngineInstance + (off)))
/* Sets the raster of a texture through the device callback (NULL clears it); returns the texture or NULL. */
void *RwTextureSetRaster(void *tex, void *raster)
{
  if (raster) return ((Fn3)ENG(0x68, void *))(tex, raster, 0) ? tex : 0;
  FLD(tex, void *, 0) = 0;
  return tex;
}
