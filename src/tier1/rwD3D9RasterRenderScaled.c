/* compile: /G5 */
#include "mfl_tier2.h"
/*004e0410*/ extern void __cdecl rwD3D9RasterRenderGeneric(void *a, void *b, int c, int d);
/* Renders a raster with the generic routine, flags (1, 1). */
void rwD3D9RasterRenderScaled(void *a, void *b)
{
  rwD3D9RasterRenderGeneric(a, b, 1, 1);
}
