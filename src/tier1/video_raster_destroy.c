#include "mfl_tier2.h"
/*004d77e0*/ extern RwBool RwRasterDestroy(struct RwRaster *p0);
/*00771038*/ extern undefined4 g_app_771038;


/* video_raster_destroy */

/* "destroys g_app_771038" */

undefined4 video_raster_destroy(void)
{
  if (g_app_771038 != (RwRaster *)0x0) {
    RwRasterDestroy(g_app_771038);
    g_app_771038 = (RwRaster *)0x0;
  }
  return 1;
}
