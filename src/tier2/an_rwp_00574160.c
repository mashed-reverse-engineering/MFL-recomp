/* rwp_trilist_00574160 (0x00574160): RwpTrilist vtable slot (5f680c): mass properties of a trilist from its bounding box */
#include "mfl_tier2.h"

extern int rwp_sphere_0056c340(void *shape, void *xf, int local, float *box);

/* The shape's local box (max corner at box[0..2], min at box[4..6]) through the shape vtable (56c340).  Each extent is scaled by 0.4
   and grown by the margin (+0x4c); inertia[0..2] = the diagonal of a box-like inertia (b^2 + c^2) / 3 ..., inertia[3..6] = identity
   rotation (0, 0, 0, 1), centre = the box centre.  Returns the volume term 8abc (0 when the shape has no box). */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
float rwp_trilist_00574160(float *shape, float *inertia, float *centre)
{
  float box[8];
  float ex, ey, ez, m, a, b, c, a2, b2, c2;

  if (rwp_sphere_0056c340(shape, 0, 1, box) == 0)
    return 0.0f;
  ex = box[0] - box[4];
  m = shape[19];
  ey = box[1] - box[5];
  inertia[3] = 0.0f;
  inertia[4] = 0.0f;
  inertia[5] = 0.0f;
  inertia[6] = 1.0f;
  ez = box[2] - box[6];
  a = ex * 0.4f + m;
  b = ey * 0.4f + m;
  c = ez * 0.4f + m;
  c2 = c * c;
  b2 = b * b;
  inertia[0] = (b2 + c2) * 0.33333334f;
  a2 = a * a;
  inertia[1] = (a2 + c2) * 0.33333334f;
  inertia[2] = (a2 + b2) * 0.33333334f;
  centre[0] = ex * 0.5f + box[4];
  centre[1] = ey * 0.5f + box[5];
  centre[2] = ez * 0.5f + box[6];
  return c * b * a * 8.0f;
}
