#include "mfl_tier2.h"
#include <math.h>

/* Support extent of a box along the direction dir: the half sizes are at 16..18 of the shape record.  Writes -extent / +extent to lo / hi
   and returns 1. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_box_0056d8c0(float *box, float *dir, float *lo, float *hi)
{
  float ext_yz;
  float abs_x;
  float ext;

  ext_yz = (float)fabs(dir[2]) * box[18] + (float)fabs(dir[1]) * box[17];
  abs_x = (float)fabs(dir[0]);
  ext = ext_yz + (float)fabs(dir[0]) * box[16];
  hi[0] = ext;
  lo[0] = -ext;
  return 1;
}
