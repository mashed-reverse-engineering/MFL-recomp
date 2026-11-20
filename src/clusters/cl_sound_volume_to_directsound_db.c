// cluster-entries: sound_volume_to_directsound_db
/* Was a cluster of the DirectShow IBasicAudio volume code; the movie audio volume belongs to the mfl_video backend now (mfl_video_set_volume). */
#include "mfl_tier2.h"
#include "mfl_video.h"
extern mfl_video *g_movie_video;
/* sound_volume_to_directsound_db */

/* Sets the movie sound volume (0..1 linear; sound_music_fade_step_b calls it only while a movie is active: the main-menu music is the sound
   track of frontend.mpg). The original converted to 1/100 dB for IBasicAudio::put_Volume; the backend maps it (mfl_video.h).
   Returns 1 like the original after a set (callers ignore it). */
int sound_volume_to_directsound_db(float volume)
{
  mfl_video_set_volume(g_movie_video, volume);
  return 1;
}
