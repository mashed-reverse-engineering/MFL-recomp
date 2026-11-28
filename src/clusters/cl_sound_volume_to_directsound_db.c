// cluster-entries: sound_volume_to_directsound_db
/* Was a cluster of the DirectShow IBasicAudio volume code; the movie audio volume belongs to the mfl_video backend now (mfl_video_set_volume). */
#include "mfl_tier2.h"
#include "mfl_video.h"
#include "mfl_fix.h"
extern mfl_video *g_movie_video;
/* sound_volume_to_directsound_db */

#if MFL_FIX_PREFS
/*00771a1c*/ extern int g_pump_windows_771a1c;      /* window procedure: 1 = the game window is active */
static float s_volume;          /* the game's last volume ... */
static mfl_video *s_volume_of;  /* ... and the movie it was for (another movie still has the backend's start value 1.0) */

/* mute_unfocused=1: the movie sound (menu music) is 0 while the game window is not active */
static float movie_volume(float volume)
{
  return mfl_pref(MFL_PREF_MUTE_UNFOCUSED) && g_pump_windows_771a1c != 1 ? 0.0f : volume;
}

/* window procedure, WM_ACTIVATE: with the key on, the movie sound goes off / gets the game's volume back; key off = nothing */
void mfl_movie_mute_refresh(void)
{
  if (mfl_pref(MFL_PREF_MUTE_UNFOCUSED))
    mfl_video_set_volume(g_movie_video, movie_volume(s_volume_of == g_movie_video ? s_volume : 1.0f));
}
#endif

/* Sets the movie sound volume (0..1 linear; sound_music_fade_step_b calls it only while a movie is active: the main-menu music is the sound
   track of frontend.mpg). The original converted to 1/100 dB for IBasicAudio::put_Volume; the backend maps it (mfl_video.h).
   Returns 1 like the original after a set (callers ignore it). */
int sound_volume_to_directsound_db(float volume)
{
#if MFL_FIX_PREFS
  s_volume = volume;
  s_volume_of = g_movie_video;
  mfl_video_set_volume(g_movie_video, movie_volume(volume));
#else
  mfl_video_set_volume(g_movie_video, volume);
#endif
  return 1;
}
