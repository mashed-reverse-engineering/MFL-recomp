/* rwp_shape_local_point_to_world (0x0056c0b0): object local point moved to world space with a 4x3 matrix */
#include "mfl_tier2.h"

typedef struct MflSphereTypeVt {
  void *slot[2];
  float *(*point_in_local)();   /* 0x8: (obj, out) -> pointer to the local point */
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
  float rot_x, rot_y, rot_z;
  volatile float rot_z_f;      /* a frame float in the original (rounded to float before the translation is added) */

  p = obj->type->point_in_local(obj, out);
  if (p && m) {
    rot_x = m[8] * p[2];
    rot_x = rot_x + m[4] * p[1];
    rot_x = rot_x + m[0] * p[0];
    rot_y = m[9] * p[2];
    rot_y = rot_y + m[1] * p[0];
    rot_y = rot_y + m[5] * p[1];
    rot_z = m[10] * p[2];
    rot_z = rot_z + m[2] * p[0];
    rot_z = rot_z + m[6] * p[1];
    rot_z_f = rot_z;
    p[0] = m[12] + rot_x;
    p[1] = m[13] + rot_y;
    p[2] = m[14] + rot_z_f;
  }
  return p;
}
