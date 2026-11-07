/* rwa_math_convert_linear_to_log_005bf2b0 (0x005bf2b0): math (RwaMathConvertLinearToLog): linear gain -> millibels, 2000 * log10(gain) chopped to int, clamped to <= 0; -10000 below 1e-5 */
#include "mfl_tier2.h"
#include <math.h>

#define INT32_INDEFINITE ((int)0x80000000)
#define CHOP(v) (((v) >= -2147483648.0f && (v) < 2147483648.0f) ? (int)(v) : INT32_INDEFINITE)

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
int rwa_math_convert_linear_to_log_005bf2b0(float linear)
{
  float mb;
  int v;

  if (linear < 1e-5f)
    return -10000;
  mb = (float)(log10(linear) * 2000.0f);
  v = CHOP(mb);
  return v > 0 ? 0 : v;
}
