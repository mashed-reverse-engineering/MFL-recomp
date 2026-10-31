#include "mfl_tier2.h"

/* acc[k] += scale * v[k] for the nine floats of a 3x3 matrix. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_aggregate_00584f40(float *acc, float *v, float scale)
{
  acc[0] = scale * v[0] + acc[0];
  acc[1] = v[1] * scale + acc[1];
  acc[2] = v[2] * scale + acc[2];
  acc[3] = v[3] * scale + acc[3];
  acc[4] = v[4] * scale + acc[4];
  acc[5] = v[5] * scale + acc[5];
  acc[6] = v[6] * scale + acc[6];
  acc[7] = v[7] * scale + acc[7];
  acc[8] = v[8] * scale + acc[8];
}
