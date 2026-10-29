/* compile: /G5 */
#include "mfl_tier2.h"
/*00947144*/ extern int g_rw_947144;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* TRUE when the D3D9 raster extension has compression bits set. */
int RwD3D9RasterIsCompressed(void *r)
{
  return (FLD(r, unsigned char, g_rw_947144 + 0xa) & 0xf0) != 0;
}
