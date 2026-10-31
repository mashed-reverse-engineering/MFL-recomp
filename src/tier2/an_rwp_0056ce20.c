#include "mfl_tier2.h"

extern float RwV3dLength(float *v);

/* Mass centre and bounding radius of a box record (half sizes at 16..18, margin at 19): centre (0, 0, 0), radius = |half sizes| + margin.
   Returns `out`. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float *rwp_box_0056ce20(float *box, float *out)
{
  float half[3];

  half[0] = box[16];
  half[1] = box[17];
  half[2] = box[18];
  out[2] = 0.0f;
  out[1] = 0.0f;
  out[0] = 0.0f;
  out[3] = RwV3dLength(half) + box[19];
  return out;
}
