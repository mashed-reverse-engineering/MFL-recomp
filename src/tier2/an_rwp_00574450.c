/* rwp_trilist_00574450 (0x00574450): RwpTrilist vtable slot (5f6820): interval of the trilist's bounding sphere along a direction */
#include "mfl_tier2.h"

extern void rwp_trilist_00574050(float *shape, float *sphere);

/* Bounding sphere (centre xyz, radius) from 574050, projected onto `dir`: *lo = centre.dir - radius, *hi = centre.dir + radius.
   Returns 1. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwp_trilist_00574450(float *shape, float *dir, float *lo, float *hi)
{
  float sphere[4];
  double c;          /* x87 register value of the original; one operation per statement keeps its (y, x, z) order */

  rwp_trilist_00574050(shape, sphere);
  c = (double)dir[1] * sphere[1];
  c = c + (double)sphere[0] * dir[0];
  c = c + (double)dir[2] * sphere[2];
  *lo = c;
  *hi = c + sphere[3];
  *lo = *lo - sphere[3];
  return 1;
}
