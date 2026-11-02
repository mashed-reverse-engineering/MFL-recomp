/* rwp_capsule_support_extent (0x00574c90): support extent of a capsule along a direction */
#include "mfl_tier2.h"
#include <math.h>

/* Support extent of a capsule along the direction dir (the radius is at 16 of the shape record): writes -extent / +extent to lo / hi, returns 1. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_capsule_support_extent(float *cap, float *dir, float *lo, float *hi)
{
  float ext;

  ext = (float)fabs(dir[2]) * cap[16];
  hi[0] = ext;
  lo[0] = -ext;
  return 1;
}
