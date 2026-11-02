/* rwp_null_shape_mass_props (0x0058c860): mass properties of the null shape */
#include "mfl_tier2.h"

/* Mass properties of the null shape: unit inertia diagonal and last row (0, 0, 1) in the 7 float record `inertia`, centre of mass (0, 0, 0)
   in `com`, returns 0.0 (volume). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float rwp_null_shape_mass_props(int a1, float *inertia, float *com)
{
  inertia[0] = 1.0f;
  inertia[1] = 1.0f;
  inertia[2] = 1.0f;
  inertia[3] = 0.0f;
  inertia[4] = 0.0f;
  inertia[5] = 0.0f;
  inertia[6] = 1.0f;
  com[2] = 0.0f;
  com[1] = 0.0f;
  com[0] = 0.0f;
  return 0.0f;
}
