/* rwa_soft_get_rate_param_005d9a50 (0x005d9a50): software output: parameter callback, the float at (+0x20)->+0x60 as an integer */
#include "mfl_tier2.h"

#define INT32_INDEFINITE (-2147483647 - 1)      /* what a 32-bit fistp stores for NaN and out-of-range values */

/* The original truncates with a 32-bit fistp under chop rounding: in range that is (int); outside it (and for NaN) the
   integer indefinite, where (int) through _ftol would keep the low 32 bits of a 64-bit result.  Returns obj. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_soft_get_rate_param_005d9a50(char *obj, int param, int *out)
{
  float rate = *(float *)(*(char **)(obj + 0x20) + 0x60);

  (void)param;
  *out = (rate >= -2147483648.0f && rate < 2147483648.0f) ? (int)rate : INT32_INDEFINITE;
  return obj;
}
