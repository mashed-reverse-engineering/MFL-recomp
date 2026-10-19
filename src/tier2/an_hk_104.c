#include "mfl_tier2.h"
/* FUN_004c4720 */

/* Linear interpolation of a 3-component float vector (inferred):
   out = start + t * (end - start). Writes the result to out; out may be
   end but must not be start (start is read after out is written).
   Returns nothing. */
void FUN_004c4720(float *out,float *start,float *end,float t)

{
  float delta_x;
  float delta_y;
  float delta_z;

  *out = *end - *start;
  out[1] = end[1] - start[1];
  out[2] = end[2] - start[2];
  delta_x = *out;
  *out = t * delta_x;
  delta_y = out[1];
  out[1] = t * delta_y;
  delta_z = out[2];
  out[2] = t * delta_z;
  *out = t * delta_x + *start;
  out[1] = t * delta_y + start[1];
  out[2] = t * delta_z + start[2];
  return;
}


