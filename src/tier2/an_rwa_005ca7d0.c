/* rwa_dxaudio_voice_set_wave_005ca7d0 (0x005ca7d0): DirectSound voice (_rwaDXAudioVoiceSetWave): stop the voice, drop its buffer and attach the new wave (software mode: to the soft mixer voice; hardware mode: a duplicate of the wave's DirectSound buffer) */
#include "mfl_tier2.h"

/*005ca690*/ extern int _rwaDXAudioVoiceGetPlaying(void *voice);
/*005ca650*/ extern void rwa_dxaudio_voice_set_trigger_005ca650(char *voice, int trigger);
/*005d8540*/ extern void rwa_soft_voice_set_wave_005d8540(char *voice, void *samples);
/*005cd2f0*/ extern int _rwaDSBufferDestroy(void *buffer);
/*005cd320*/ extern int _rwaDSBufferDuplicate(void *dsound, void *source, void *buffer);

#define VOICE_OUTPUT(v)  (*(char **)((v) + 0x94))      /* the output; +0x50 flags, +0x54 its DirectSound object */
#define OUTPUT_SOFTWARE  0x8                           /* output flag: voices are mixed in software */
#define VOICE_WAVE(v)    (*(char **)((v) + 0x84))
#define VOICE_BUFFER(v)  ((v) + 0xdc)
#define VOICE_SOFT(v)    (*(char **)((v) + 0x11c))     /* the soft mixer voice */
#define VOICE_FLAGS(v)   (*(RwUInt32 *)((v) + 0x28))
#define VF_POSITION_SET  0x20
#define VF_WAVE_CHANGED  0x1000

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_dxaudio_voice_set_wave_005ca7d0(char *voice, char *wave)
{
  int soft;

  if (_rwaDXAudioVoiceGetPlaying(voice))
    rwa_dxaudio_voice_set_trigger_005ca650(voice, 0);
  soft = (*(RwUInt32 *)(VOICE_OUTPUT(voice) + 0x50) >> 3) & 1;
  if (soft)
    rwa_soft_voice_set_wave_005d8540(VOICE_SOFT(voice), wave ? *(void **)(wave + 0x4c) : 0);
  if (VOICE_WAVE(voice) && !soft)
    _rwaDSBufferDestroy(VOICE_BUFFER(voice));
  VOICE_WAVE(voice) = 0;
  *(float *)(voice + 0x38) = 1.0f;
  *(float *)(voice + 0x3c) = 1.0f;
  if (!wave)
    return;
  if (!soft && !_rwaDSBufferDuplicate(*(void **)(VOICE_OUTPUT(voice) + 0x54), wave + 0x5c, VOICE_BUFFER(voice)))
    return;
  VOICE_WAVE(voice) = wave;
  VOICE_FLAGS(voice) = (VOICE_FLAGS(voice) & ~VF_POSITION_SET) | VF_WAVE_CHANGED;
}
