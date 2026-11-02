/* rwp_cylinder_support_extent (0x005705c0): extent of a cylinder along a unit direction */
#include "mfl_tier2.h"
#include <math.h>

typedef struct MflCylSize {
  unsigned char pad00[0x40];
  float half;   /* 0x40 */
  float radius;   /* 0x44 */
} MflCylSize;

/* Extent of a cylinder (half height, radius) along the unit direction v: hi = sqrt(|1 - vz^2|) * radius + vz * half, lo = -hi.  Returns 1. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
int rwp_cylinder_support_extent(MflCylSize *p, float *v, float *lo, float *hi)
{
  float vz = v[2];
  float e;

  e = (float)sqrt(fabs(1.0f - vz * vz)) * p->radius + vz * p->half;
  *lo = -e;
  *hi = e;
  return 1;
}
