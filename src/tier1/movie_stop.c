#include "mfl_tier2.h"
#include "mfl_video.h"
/*004a3860*/ extern int movie_renderer_release();
/*004d77e0*/ extern RwBool RwRasterDestroy(struct RwRaster *p0);
/*0076e890*/ extern undefined4 g_app_76e890;
/*0076e8a4*/ extern char *g_app_76e8a4;
extern mfl_video *g_movie_video;
/* movie_stop */

/* stops the movie (mfl_video backend), releases it and destroys the raster */

void __cdecl movie_stop(undefined4 unused_1)
{
  mfl_video_set_volume(g_movie_video,0.0f);
  movie_renderer_release();
  RwRasterDestroy((struct RwRaster *)g_app_76e8a4);
  g_app_76e8a4 = (char *)(RwRaster *)0x0;
  g_app_76e890 = 0;
  return;
}
