/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Reads the x and y offset of a raster (16 bit each); returns the raster. */
void *RwRasterGetOffset(void *r, short *x, short *y)
{
  *x = FLD(r, short, 0x1c);
  *y = FLD(r, short, 0x1e);
  return r;
}
