/* compile: /G5 */
#include "mfl_tier2.h"
/*004d78c0*/ extern int __cdecl RwRasterShowRaster(void *r, void *a, int b);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Shows the camera's frame buffer raster on the window; returns the camera or NULL. */
void *RwCameraShowRaster(void *cam, void *a, int b)
{
  return RwRasterShowRaster(FLD(cam, void *, 0x60), a, b) ? cam : 0;
}
