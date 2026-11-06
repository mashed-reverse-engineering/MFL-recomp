/* rwa_dxaudio_voice_get_reverb_send_005cb1b0 (0x005cb1b0): DirectX audio voice parameter callback: the reverb send gain into *value; x87 */
#include "mfl_tier2.h"

/*005ca8f0*/ extern float _rwaDXAudioVoiceGetReverbSendGain(char *voice);

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_dxaudio_voice_get_reverb_send_005cb1b0(char *voice, RwUInt32 param, float *value)
{
  *value = _rwaDXAudioVoiceGetReverbSendGain(voice);
  return voice;
}
