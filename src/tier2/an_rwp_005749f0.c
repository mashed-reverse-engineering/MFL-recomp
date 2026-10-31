#include "mfl_tier2.h"

/* Centre of mass (0, 0, 0) and bounding radius (cap[19] + cap[16]) of a capsule record. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_capsule_005749f0(float *cap, float *out)
{
  out[2] = 0.0f;
  out[1] = 0.0f;
  out[0] = 0.0f;
  out[3] = cap[19] + cap[16];
}
