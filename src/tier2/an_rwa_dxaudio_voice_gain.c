/* RenderWare Audio (Criterion Software) DirectX audio voice: reverb send gain and a gain parameter getter, MFL.exe
 * 0x005ca890-0x005ca940 and 0x005caa50.  Written from the MFL.exe disassembly (objdump).
 * Voice layout used here: +0x28 dirty flags of the DirectSound path, +0x40 reverb send in millibels, +0x94 output description
 * (byte +0x50: output kind bits; 8 = the path with its own 3D state, 7 = the DirectSound kinds), +0xcc a gain in millibels,
 * +0x11c the 3D state of the kind 8 path (+0x40 reverb send, linear; +0xcc its dirty flags). */
#include "mfl_tier2.h"

/*005bf2b0*/ extern int RwaMathConvertLinearToLog(float linear);
/*004b2c30*/ extern double __cdecl _pow(double x, double y);      /* the CRT function (cl would expand pow() to _CIpow) */

#define OUTPUT_KIND(voice) (*(RwUInt8 *)(*(char **)((voice) + 0x94) + 0x50))
#define DIRTY_REVERB_SEND 0x10

/* TIER2: unvalidated 2026-11-12 authority=diffemu */
void _rwaDXAudioVoiceSetReverbSendGain(char *voice, RwReal gain)
{
  if (OUTPUT_KIND(voice) & 8) {
    *(RwReal *)(*(char **)(voice + 0x11c) + 0x40) = gain;
    *(RwUInt32 *)(*(char **)(voice + 0x11c) + 0xcc) |= DIRTY_REVERB_SEND;
  } else if (OUTPUT_KIND(voice) & 7) {
    *(RwInt32 *)(voice + 0x40) = RwaMathConvertLinearToLog(gain);
    *(RwUInt32 *)(voice + 0x28) |= DIRTY_REVERB_SEND;
  }
}

/* Linear reverb send gain: stored as it is on the kind 8 path, 10 ^ (millibels / 2000) on the DirectSound kinds, else 0. */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
float _rwaDXAudioVoiceGetReverbSendGain(char *voice)
{
  if (OUTPUT_KIND(voice) & 8) return *(float *)(*(char **)(voice + 0x11c) + 0x40);
  if (OUTPUT_KIND(voice) & 7) return _pow(10.0, *(RwInt32 *)(voice + 0x40) * 0.0005f);
  return 0.0f;
}

/* Parameter getter (voice, parameter, value out): the millibel gain at +0xcc as a linear factor.  Returns the voice.
   (The symbol table calls it _RwaDXAudioVoiceSetMaxDistance, which does not fit.) */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
char *rwa_dxaudio_voice_get_linear_gain_005caa50(char *voice, void *param, RwReal *value)
{
  *value = _pow(10.0, *(RwInt32 *)(voice + 0xcc) * 0.0005f);
  return voice;
}
