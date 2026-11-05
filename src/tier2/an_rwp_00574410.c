/* rwp_trilist_00574410 (0x00574410): RwpTrilist vtable slot (5f6824): half the bounding radius beyond the margin */
#include "mfl_tier2.h"

extern void rwp_trilist_00574050(float *shape, float *sphere);

/* Bounding sphere of the trilist shape (centre xyz, radius) from 574050; returns half of the radius minus the margin at
   +0x4c when the radius exceeds it, else half the radius. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
float rwp_trilist_00574410(float *shape)
{
  float sphere[4];
  float r;

  rwp_trilist_00574050(shape, sphere);
  r = sphere[3];
  if (sphere[3] > shape[19])
    r = r - shape[19];
  return r * 0.5f;
}
