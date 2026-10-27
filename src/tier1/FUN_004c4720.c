#include "mfl_tier2.h"
/* FUN_004c4720 */
/* Linear interpolation of a 3-component float vector (inferred):
   out = start + t * (end - start). Writes the result to out; out may be
   end but must not be start (start is read after out is written).
   Returns nothing. */
void FUN_004c4720(float *out,float *start,float *end,float t)
{
  *out = *end - *start;
  out[1] = end[1] - start[1];
  out[2] = end[2] - start[2];
  *out = t * *out;
  out[1] = t * out[1];
  out[2] = t * out[2];
  *out = *out + *start;
  out[1] = out[1] + start[1];
  out[2] = out[2] + start[2];
  return;
}
