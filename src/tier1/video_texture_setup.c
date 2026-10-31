#include "mfl_tier2.h"
/*004a3a20*/ extern int movie_renderer_create();
/*004d7950*/ extern struct RwRaster * RwRasterCreate(RwInt32 p0, RwInt32 p1, RwInt32 p2, RwInt32 p3);
/*0076e890*/ extern undefined4 g_app_76e890;
/*0076e894*/ extern undefined4 g_app_76e894;
/*0076e8a4*/ extern undefined4 g_app_76e8a4;


#define RWRASTER_FORMAT_DEFAULT_CAMERATEXTURE 0x84

/* Initializes the video playback texture raster and underlying movie renderer. */
void video_texture_setup(undefined4 param_1_unused,undefined4 movie_resource_id,undefined4 display_flag)
{
  /* Create raster with default dimensions and camera texture format */
  g_app_76e8a4 = RwRasterCreate(0,0,0,RWRASTER_FORMAT_DEFAULT_CAMERATEXTURE);
  /* Initialize the movie rendering pipeline using the provided movie identifier */
  /* active only when the movie player could open the movie (the null backend never does: the movie is skipped) */
  g_app_76e890 = (movie_renderer_create(movie_resource_id) == 0);
  g_app_76e894 = display_flag;
  return;
}
