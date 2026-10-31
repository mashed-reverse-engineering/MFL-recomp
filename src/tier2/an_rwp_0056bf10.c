#include "mfl_tier2.h"

/*0056c1c0*/ extern float rwp_sphere_0056c1c0(int obj, void *out, float *centre);

typedef struct RwpSupportOut {
  float v[3];   /* 0x0 */
  float q[4];   /* 0xc */
} RwpSupportOut;
typedef struct RwpSphereRef {
  int obj;   /* 0x0 */
  float centre[3];   /* 0x4 */
  float scale;   /* 0x10 */
  float v[3];   /* 0x14 */
  float q[4];   /* 0x20 */
  float extent;   /* 0x30 */
  unsigned char pad34[0x14];
  unsigned int flags;   /* 0x48 */
} RwpSphereRef;

/* Fills ref (centre, v, q from rwp_sphere_0056c1c0 on ref->obj) for a requested scale: a negative scale is turned into -(scale * r), r = the
   factor returned by the callee.  With a nonzero keep flag the v vector is scaled by scale when it is larger than FLT_MIN and flag bit 1 is set;
   without it, the largest component of v times scale becomes ref->extent and bit 1 is cleared (both only when scale > FLT_MIN).
   Returns scale / r, or FLT_MAX when r < FLT_MIN. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float rwp_sphere_0056bf10(RwpSphereRef *ref, float scale, int keep)
{
  RwpSupportOut o;
  float c[3];
  double r;
  float mx;

  r = rwp_sphere_0056c1c0(ref->obj, &o, c);
  if (scale < 0.0f) scale = -(scale * r);
  ref->scale = scale;
  ref->centre[0] = c[0];
  ref->centre[1] = c[1];
  ref->centre[2] = c[2];
  if (keep != 0) {
    if (scale > 1.17549435e-38f) {
      o.v[0] = o.v[0] * scale;
      o.v[1] = o.v[1] * scale;
      o.v[2] = o.v[2] * scale;
    }
    ref->v[0] = o.v[0];
    ref->v[1] = o.v[1];
    ref->v[2] = o.v[2];
    ref->q[0] = o.q[0];
    ref->q[1] = o.q[1];
    ref->q[2] = o.q[2];
    ref->q[3] = o.q[3];
    ref->flags |= 2;
  } else if (scale > 1.17549435e-38f) {
    if (o.v[0] > o.v[1]) {
      mx = o.v[0] > o.v[2] ? o.v[0] : o.v[2];
    } else {
      mx = o.v[2] > o.v[1] ? o.v[2] : o.v[1];
    }
    ref->flags &= ~2;
    ref->extent = mx * scale;
  }
  if (r < 1.17549435e-38f) return 3.4028235e38f;
  return scale / r;
}
