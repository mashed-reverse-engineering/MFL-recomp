/* compile: /G5 */
#include "mfl_tier2.h"
/*007d280c*/ extern void *g_rw_7d280c;
/*004e90b0*/ extern void *__cdecl RwFreeListCreate(int a, int b, int c, int d);
/* Module open: creates the raster extension free list (8 byte entries, 0x7f per block). */
void rwD3D9RasterOpen(void)
{
  g_rw_7d280c = RwFreeListCreate(8, 0x7f, 4, 0x30411);
}
