/* compile: /G5 */
#include "mfl_tier2.h"
/*00947144*/ extern int g_rw_947144;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Raster plugin constructor: clears the D3D9 raster extension field +0x1c. */
void rwD3D9RasterCtor(void *r)
{
  FLD(r, int, g_rw_947144 + 0x1c) = 0;
}
