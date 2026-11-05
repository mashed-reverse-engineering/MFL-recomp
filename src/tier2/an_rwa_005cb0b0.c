/* rwa_dxaudio_voice_set_loop_param_005cb0b0 (0x005cb0b0): input-parameter setter of the DXAudio voice: loop flag */
#include "mfl_tier2.h"

/*005ca780*/ extern void FUN_005ca780(void *voice, int loop);      /* _rwaDXAudioVoiceSetLoop */

/* Parameter callback (obj, param id, value in): returns the object. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_dxaudio_voice_set_loop_param_005cb0b0(void *voice, int param, int *value)
{
  FUN_005ca780(voice, *value);
  return voice;
}
