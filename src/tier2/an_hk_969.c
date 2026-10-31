#include "mfl_tier2.h"
#include "mfl_video.h"
/*0076e890*/ extern undefined4 g_app_76e890;
/*0076e894*/ extern undefined4 g_app_76e894;
extern mfl_video *g_movie_video;
/* movie_renderer_poll */

/* polls the movie player once per frame; at the end of the stream clears g_app_76e890 unless the keep-active flag g_app_76e894 is set */

void movie_renderer_poll(void)

{
  if (g_movie_video != 0) {
    mfl_video_update(g_movie_video);
    if (mfl_video_finished(g_movie_video) && g_app_76e894 == 0) {
      g_app_76e890 = 0;
    }
  }
  return;
}
