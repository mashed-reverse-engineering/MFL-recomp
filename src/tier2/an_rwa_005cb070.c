/* rwa_dxaudio_voice_set_wave_pos_param_005cb070 (0x005cb070): input-parameter setter of the DXAudio voice: wave position */
#include "mfl_tier2.h"

/*005ca710*/ extern void FUN_005ca710(void *voice, int pos);      /* _rwaDXAudioVoiceSetWavePos */

/* Parameter callback (obj, param id, value in): returns the object. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_dxaudio_voice_set_wave_pos_param_005cb070(void *voice, int param, int *value)
{
  FUN_005ca710(voice, *value);
  return voice;
}
