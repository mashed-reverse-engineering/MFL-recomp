#include "mfl_tier2.h"

/* Mass properties of a sphere record (radius at 19): inertia diagonal 0.4 r^2 in the first three floats of the 7 float record, the last row
   (0, 0, 1), centre of mass (0, 0, 0) to `com`; returns the volume term 4.18879 r^3. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float rwp_sphere_0056cb50(float *sph, float *inertia, float *com)
{
  float r, rr;

  com[2] = 0.0f;
  com[1] = 0.0f;
  com[0] = 0.0f;
  inertia[3] = 0.0f;
  inertia[4] = 0.0f;
  inertia[5] = 0.0f;
  *(int *)&inertia[6] = 0x3f800000;
  r = sph[19];
  rr = r * r;
  inertia[0] = 0.4f * rr;
  inertia[1] = 0.4f * rr;
  inertia[2] = 0.4f * rr;
  return r * rr * 4.1887903f;
}
