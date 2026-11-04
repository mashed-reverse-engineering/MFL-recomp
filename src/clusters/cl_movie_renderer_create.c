// cluster-entries: movie_renderer_create
/* Was the DirectShow graph builder (cluster pilot z2); now the open of the mfl_video backend. */
#include "mfl_tier2.h"
#include "mfl_video.h"

#define E_FAIL_HRESULT ((int)0x80004005)
#define rwRASTERFORMAT888 0x0600                 /* X8R8G8B8, the format of the original renderer texture (d3dx_texture_create) */

/*004d7950*/ extern RwRaster *RwRasterCreate(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
/*004d77e0*/ extern RwBool RwRasterDestroy(RwRaster *raster);
extern int g_movie_renderer_76e8a8;          /* the movie number */
extern const char *PTR_s_toastart_pc_movies_frontend_mpg_0062ad3c[];   /* movie file names */
extern mfl_video *g_movie_video;
extern RwRaster *g_app_76e8a4;               /* the movie raster (video_texture_setup creates a 0x0 placeholder) */
extern float g_app_76e878, g_app_76e87c, g_app_76e880, g_app_76e884;     /* video size and texture size as floats (app_get_two_size_pairs) */

/* Gives the movie a texture raster: power of two sizes (at least 2) like d3dx_texture_create on a POW2 card, the sizes published for the quad
   texture coordinates (video / texture). A backend without a size (blank) keeps the placeholder raster and the zero sizes. */
static void movie_raster_create(void)
{
  int w, h, tw = 2, th = 2;
  RwRaster *raster;

  mfl_video_get_size(g_movie_video, &w, &h);
  if (w <= 0 || h <= 0) return;
  while (tw < w) tw *= 2;
  while (th < h) th *= 2;
  raster = RwRasterCreate(tw, th, 32, rwRASTERTYPETEXTURE | rwRASTERFORMAT888);
  if (raster == 0) return;
  if (g_app_76e8a4 != 0) RwRasterDestroy(g_app_76e8a4);
  g_app_76e8a4 = raster;
  g_app_76e878 = (float)w;
  g_app_76e87c = (float)h;
  g_app_76e880 = (float)tw;
  g_app_76e884 = (float)th;
}

/* Opens and starts movie `movie` in the mfl_video backend. Returns 0, or E_FAIL (no backend / open failed: the movie is skipped silently). */
int movie_renderer_create(int movie)
{
  g_movie_renderer_76e8a8 = movie;
  g_movie_video = mfl_video_open(PTR_s_toastart_pc_movies_frontend_mpg_0062ad3c[movie], 0);
  if (g_movie_video == 0) return E_FAIL_HRESULT;
  if (!mfl_video_start(g_movie_video)) { mfl_video_close(g_movie_video); g_movie_video = 0; return E_FAIL_HRESULT; }
  movie_raster_create();
  return 0;
}
