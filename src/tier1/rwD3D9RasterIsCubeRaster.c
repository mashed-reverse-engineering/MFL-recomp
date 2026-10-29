/* compile: /G5 */
#include "mfl_tier2.h"
/*00947144*/ extern int g_rw_947144;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Cube flag bits (low nibble of byte +9) of the D3D9 raster extension. */
int rwD3D9RasterIsCubeRaster(void *r)
{
  return FLD(r, unsigned char, g_rw_947144 + 9) & 0xf;
}
