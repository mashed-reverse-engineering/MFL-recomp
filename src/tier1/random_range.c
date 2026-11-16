#include "mfl_tier2.h"
#include "mfl_fix.h"
/*00544e80*/ extern RwUInt32 __cdecl RpRandom(void);
/*005de498*/ extern float k_f_4_656613en10;
/* Returns a uniform random float in [lo, hi) from RpRandom. */
float random_range(float lo, float hi)
{
  unsigned int r = RpRandom() & 0x7fffffff;
  return (hi - lo) * ((float)r * k_f_4_656613en10) + lo;
}

#if MFL_FIX_DETERMINISTIC_AUDIO_RNG
#include "mfl_audio_rng.h"
/* The audio-stream copies of random_range and rp_random_int_range (an_rw_math_4c4120.c): the same bodies, drawing from the sound stream
   instead of RpRandom.  The sound files call them through the renames in mfl_fix.h. */
static MflAudioRng g_mfl_audio_rng;

float mfl_audio_random_range(float lo, float hi)
{
  unsigned int r = mfl_audio_rng_next(&g_mfl_audio_rng) & 0x7fffffff;
  return (hi - lo) * ((float)r * k_f_4_656613en10) + lo;
}

int mfl_audio_rp_random_int_range(int lo, int hi)
{
  return mfl_audio_rng_next(&g_mfl_audio_rng) % (hi - lo + 1) + lo;
}
#endif
