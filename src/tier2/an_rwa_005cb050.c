/* rwa_dxaudio_voice_get_playing_param_005cb050 (0x005cb050): output-parameter getter of the DXAudio voice: "playing" */
#include "mfl_tier2.h"

/*005ca690*/ extern int FUN_005ca690(void *voice);      /* _rwaDXAudioVoiceGetPlaying */

/* Parameter callback (obj, param id, value out): returns the object. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_dxaudio_voice_get_playing_param_005cb050(void *voice, int param, int *value)
{
  *value = FUN_005ca690(voice);
  return voice;
}
