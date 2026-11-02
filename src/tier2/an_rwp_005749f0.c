/* rwp_capsule_centre_and_radius (0x005749f0): centre of mass and bounding radius of a capsule record */
#include "mfl_tier2.h"

/* Centre of mass (0, 0, 0) and bounding radius (cap[19] + cap[16]) of a capsule record. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_capsule_centre_and_radius(float *cap, float *out)
{
  out[2] = 0.0f;
  out[1] = 0.0f;
  out[0] = 0.0f;
  out[3] = cap[19] + cap[16];
}
