/* compile: /G5 */
#include "mfl_tier2.h"
/* Stores the raster in the texture; returns TRUE. */
int rwD3D9TextureSetRaster(void *t, void *r)
{
  *(void **)t = r;
  return 1;
}
