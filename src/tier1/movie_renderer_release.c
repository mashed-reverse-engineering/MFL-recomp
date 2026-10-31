#include "mfl_tier2.h"
#include "mfl_video.h"
extern mfl_video *g_movie_video;
/* movie_renderer_release */
/* closes the movie player (mfl_video backend) */
void movie_renderer_release(void)
{
  mfl_video_close(g_movie_video);
  g_movie_video = 0;
}
