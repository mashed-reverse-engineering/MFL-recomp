#include "mfl_tier2.h"
#include "mfl_video.h"
/*004a37f0*/ extern int movie_renderer_poll();
extern mfl_video *g_movie_video;
/* movie_poll_and_set_surface */

/* polls the movie player; returns 0 while there is no movie (the play loop then draws nothing).
   TODO(first real backend): mfl_video_acquire_frame + copy the pixels into the raster (docs/VIDEO_INTERFACE.md, Frame transfer). */

undefined4 __cdecl movie_poll_and_set_surface(undefined4 unused_1,undefined4 unused_2)
{
  if (g_movie_video == 0) return 0;
  movie_renderer_poll();
  return 1;
}
