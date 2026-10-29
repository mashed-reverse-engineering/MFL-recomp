/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Makes a raster a view of a parent raster: copies the surface data pointers and flags, clears the lock state; returns TRUE. */
int rwD3D9RasterSubRaster(void *r, void *p)
{
  FLD(r, int, 0x18) = FLD(p, int, 0x18);
  FLD(r, int, 0x14) = FLD(p, int, 0x14);
  FLD(r, char, 0x20) = FLD(p, char, 0x20);
  FLD(r, char, 0x23) = FLD(p, char, 0x23);
  FLD(r, int, 4) = 0;
  return 1;
}
