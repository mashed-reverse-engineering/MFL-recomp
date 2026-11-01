#include "mfl_tier2.h"

typedef struct MflCylMass {
  unsigned char pad00[0x40];
  float half;   /* 0x40 */
  float radius;   /* 0x44 */
  unsigned char pad48[4];
  float margin;   /* 0x4c */
} MflCylMass;

/* Mass properties of a cylinder with margin (unit density): centre of mass = 0, inertia diagonal (Ixx = Iyy = (4 H^2 + 3 r^2) / 12, Izz = r^2 / 2 with
   H = 2 (half + margin), r = radius + margin), the other inertia entries 0 and the last (inverse scale) 1.  Returns the volume pi r^2 H. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
float rwp_cylinder_0056fea0(MflCylMass *p, float *inertia, float *com)
{
  float r, h, hh, rr, ixx;

  com[2] = 0;
  com[1] = 0;
  com[0] = 0;
  inertia[3] = 0;
  inertia[4] = 0;
  inertia[5] = 0;
  inertia[6] = 1.0f;
  r = p->radius + p->margin;
  h = p->half + p->margin;
  h = h + h;
  hh = h * h;
  rr = r * r;
  ixx = (hh * 4.0f + rr * 3.0f) * 0.083333336f;
  inertia[0] = ixx;
  inertia[1] = ixx;
  inertia[2] = rr * 0.5f;
  return r * (h * r) * 3.1415927f;
}
