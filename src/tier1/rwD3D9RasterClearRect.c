/* compile: /G5 */
#include "mfl_tier2.h"
/*004e0dd0*/ extern void __cdecl D3D9RasterClearGeneric(void *a, void *b);
/* Forwards the clear to the generic raster clear (drops the first argument). */
void rwD3D9RasterClearRect(void *a, void *b, void *c)
{
  D3D9RasterClearGeneric(b, c);
}
