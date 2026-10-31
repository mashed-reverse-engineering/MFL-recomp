// cluster-entries: sound_volume_to_directsound_db
/* Was a cluster of the DirectShow IBasicAudio volume code; the movie audio volume belongs to the mfl_video backend now (mfl_video_set_volume). */
#include "mfl_tier2.h"
/* sound_volume_to_directsound_db */

/* 0 = the movie sound has no volume control here (called from sound_music_fade_step_b only while a movie is active) */
undefined4 sound_volume_to_directsound_db(void)

{
  return 0;
}
