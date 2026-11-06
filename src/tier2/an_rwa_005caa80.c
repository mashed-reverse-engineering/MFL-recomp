/* rwa_dxaudio_voice_set_macro_fx_gain_005caa80 (0x005caa80): DirectX audio voice (_RwaDXAudioVoiceSetMacroFXGain): gain * 100 as an integer, mark dirty; x87 */
#include "mfl_tier2.h"

#define INT32_INDEFINITE (-2147483647 - 1)      /* what a 32-bit fistp stores for NaN and out-of-range values */
/* The original converts with a 32-bit fistp under chop rounding (v: a float variable). */
#define CHOP(v) (((v) >= -2147483648.0f && (v) < 2147483648.0f) ? (int)(v) : INT32_INDEFINITE)

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_dxaudio_voice_set_macro_fx_gain_005caa80(char *voice, RwUInt32 param, const float *gain)
{
  float g = *gain * 100.0f;

  *(RwInt32 *)(voice + 0x118) = CHOP(g);
  *(RwUInt32 *)(voice + 0x28) |= 8;
}
