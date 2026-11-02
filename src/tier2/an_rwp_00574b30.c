/* rwp_capsule_support_point (0x00574b30): support point of a capsule along a direction */
#include "mfl_tier2.h"

/* Support point of a capsule along the direction dir: (0, 0, +-half length), the sign taken from dir[2] (NaN and 0 give +). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_capsule_support_point(float *cap, float *dir, float *out)
{
  out[0] = 0.0f;
  out[1] = 0.0f;
  out[2] = (dir[2] < 0.0f ? -1.0f : 1.0f) * cap[16];
}
