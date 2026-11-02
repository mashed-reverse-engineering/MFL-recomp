/* rwp_capsule_mass_props (0x00574a10): mass properties of a capsule record */
#include "mfl_tier2.h"

/* Mass properties of a capsule record (radius at 19, half length at 16): writes the diagonal of the inertia tensor (a 7 float record: xx, yy, zz
   at 0..2, the last row (0, 0, 1) at 3..6 as in the other shapes), the centre of mass (0, 0, 0) to `com` and returns the volume term r * (2h') * r * pi. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float rwp_capsule_mass_props(float *cap, float *inertia, float *com)
{
  float r, h, h2, rr, ixx, rh;

  r = cap[19];
  h = cap[16] + r * 0.67082f;
  inertia[3] = 0.0f;
  inertia[4] = 0.0f;
  inertia[5] = 0.0f;
  *(int *)&inertia[6] = 0x3f800000;
  h2 = h + h;
  rr = r * r;
  ixx = h2 * h2 * 0.083333336f + rr * 0.25f;
  inertia[0] = ixx;
  inertia[1] = ixx;
  inertia[2] = rr * 0.5f;
  rh = h2 * r;
  com[2] = 0.0f;
  com[1] = 0.0f;
  com[0] = 0.0f;
  return r * rh * 3.1415927f;
}
