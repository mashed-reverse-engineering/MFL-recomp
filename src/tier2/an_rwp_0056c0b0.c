/* rwp_shape_local_point_to_world (0x0056c0b0): object local point moved to world space with a 4x3 matrix */
#include "mfl_tier2.h"

typedef struct MflSphereTypeVt {
  void *slot[2];
  float *(*local_point)();   /* 0x8: (obj, out) -> pointer to the local point */
} MflSphereTypeVt;

typedef struct MflSphereObjP {
  unsigned char pad00[0x5c];
  MflSphereTypeVt *type;   /* 0x5c */
} MflSphereObjP;

/* Asks the object's type (vtable slot 2) for its local point (written to / returned as `out`) and moves it to world space with the 4x3 matrix `m`
   (row vectors; translation in the last row): out = out * m + t.  Returns the point (0 when the type has none); m == 0 leaves it untouched. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
float *rwp_shape_local_point_to_world(MflSphereObjP *obj, float *m, float *out)
{
  float *p;
  float s0, s1, t2;
  volatile float s2;      /* a frame float in the original (rounded to float before the translation is added) */

  p = obj->type->local_point(obj, out);
  if (p && m) {
    s0 = m[8] * p[2];
    s0 = s0 + m[4] * p[1];
    s0 = s0 + m[0] * p[0];
    s1 = m[9] * p[2];
    s1 = s1 + m[1] * p[0];
    s1 = s1 + m[5] * p[1];
    t2 = m[10] * p[2];
    t2 = t2 + m[2] * p[0];
    t2 = t2 + m[6] * p[1];
    s2 = t2;
    p[0] = m[12] + s0;
    p[1] = m[13] + s1;
    p[2] = m[14] + s2;
  }
  return p;
}
