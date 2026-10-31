// cluster-entries: movie_renderer_create
/* Was the DirectShow graph builder (cluster pilot z2); now the open of the mfl_video backend. */
#include "mfl_tier2.h"
#include "mfl_video.h"

#define E_FAIL_HRESULT ((int)0x80004005)

extern int g_movie_renderer_76e8a8;          /* the movie number */
extern const char *PTR_s_toastart_pc_movies_frontend_mpg_0062ad3c[];   /* movie file names */
extern mfl_video *g_movie_video;

/* Opens and starts movie `movie` in the mfl_video backend. Returns 0, or E_FAIL (no backend / open failed: the movie is skipped silently). */
int movie_renderer_create(int movie)
{
  g_movie_renderer_76e8a8 = movie;
  g_movie_video = mfl_video_open(PTR_s_toastart_pc_movies_frontend_mpg_0062ad3c[movie], 0);
  if (g_movie_video == 0) return E_FAIL_HRESULT;
  if (!mfl_video_start(g_movie_video)) { mfl_video_close(g_movie_video); g_movie_video = 0; return E_FAIL_HRESULT; }
  return 0;
}
