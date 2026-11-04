#include "mfl_tier2.h"
#include "mfl_video.h"
/*004a37f0*/ extern int movie_renderer_poll();
/*004d79f0*/ extern void *RwRasterLock(RwRaster *raster, int level, int mode);
/*004d7790*/ extern RwRaster *RwRasterUnlock(RwRaster *raster);
/*0076e8a4*/ extern RwRaster *g_app_76e8a4;
extern mfl_video *g_movie_video;
/* movie_poll_and_set_surface */

#define rwRASTERLOCKWRITE 0x01      /* RW 3.7: WRITE 1, READ 2 (_rwD3D9RasterLock 0x4dfcd0 locks READONLY | NO_DIRTY_UPDATE without bit 0) */
#define RwRaster_stride(p) (*(RwInt32 *)((char *)(p) + 0x18))     /* bytes per row, set by RwRasterLock */

/* copies the newest movie frame (32 bit BGRA) into the movie raster g_app_76e8a4 (a texture sized by movie_renderer_create). The rows go in
   bottom-up, as DirectShow delivered them: both movie quads (video_draw_textured_quad, gfx_draw_movie_rect_2d) put v = 0 at the bottom edge. */
static void movie_upload_frame(void)
{
  mfl_video_frame frame;
  RwRaster *raster = g_app_76e8a4;
  unsigned char *pixels;
  const unsigned int *src;
  unsigned int *dst;
  int x, y;

  if (!mfl_video_acquire_frame(g_movie_video, &frame)) return;
  /* the 0x0 placeholder raster of video_texture_setup stays when the backend reports no size (blank backend) */
  if (raster == 0 || RwRaster_width(raster) < frame.width || RwRaster_height(raster) < frame.height) return;
  pixels = (unsigned char *)RwRasterLock(raster, 0, rwRASTERLOCKWRITE);
  if (pixels == 0) return;
  for (y = 0; y < frame.height; y++) {
    src = (const unsigned int *)((const unsigned char *)frame.pixels + y * frame.pitch);
    dst = (unsigned int *)(pixels + (frame.height - 1 - y) * RwRaster_stride(raster));
    for (x = 0; x < frame.width; x++) dst[x] = src[x];
  }
  RwRasterUnlock(raster);
}

/* polls the movie player and uploads a new frame into the movie raster (the original pointed the raster at the renderer's D3D texture instead);
   returns 0 while there is no movie or it has already ended (the play loop then draws nothing: the blank default backend ends at once, so the
   intro movies are skipped without a drawn frame). */
undefined4 __cdecl movie_poll_and_set_surface(undefined4 unused_1,undefined4 unused_2)
{
  if (g_movie_video == 0) return 0;
  movie_renderer_poll();
  if (mfl_video_update(g_movie_video)) movie_upload_frame();     /* 1 = a frame not acquired yet (a second update in the same poll adds no frame) */
  return !mfl_video_finished(g_movie_video);
}
