#include "mfl_tier2.h"
/*004d7950*/ extern struct RwRaster * RwRasterCreate(RwInt32 p0, RwInt32 p1, RwInt32 p2, RwInt32 p3);
/*0076e898*/ extern undefined4 g_d3dx_76e898;
/*00771034*/ extern undefined4 g_app_771034;
/*00771038*/ extern undefined4 g_app_771038;
#define VIDEO_RASTER_FLAGS (rwRASTERDONTALLOCATE | rwRASTERTYPETEXTURE)

/* Creates a video texture raster without allocating pixel memory. */
undefined4 __cdecl video_raster_create(undefined4 unknown)
{
  /* Store unknown D3DX reference to application global */
  g_app_771034 = g_d3dx_76e898;
  /* Create unallocated texture raster for video */
  g_app_771038 = RwRasterCreate(0,0,0,VIDEO_RASTER_FLAGS);
  return 1;
}
