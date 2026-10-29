/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007d2184*/ extern int g_rw_7d2184;
typedef int (__cdecl *Fn2)(void *, void *);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Generates mipmaps for a texture's raster through the texture plugin callback; TRUE on success. */
int RwTextureRasterGenerateMipmaps(void *t, void *r)
{
  return FLD(RwEngineInstance + g_rw_7d2184, Fn2, 0x2c)(t, r) != 0;
}
