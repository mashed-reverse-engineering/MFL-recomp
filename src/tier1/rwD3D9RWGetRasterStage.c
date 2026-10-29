/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct { int first; int pad[5]; } RwD3D9RasterStage;
/*007d4c20*/ extern RwD3D9RasterStage g_rw_7d4c20[];
/* First field of the raster stage record of a texture stage. */
int rwD3D9RWGetRasterStage(int s)
{
  return g_rw_7d4c20[s].first;
}
