/* compile: /G5 */
#include "mfl_tier2.h"
/*00947144*/ extern int g_rw_947144;
/* Returns the has-alpha byte of the texture raster D3D9 extension; 0 without raster. */
unsigned rwD3D9TextureHasAlpha(void **p)
{
  if (*p) return *((unsigned char *)*p + 8 + g_rw_947144);
  return 0;
}
