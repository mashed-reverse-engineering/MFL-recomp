#include "mfl_tier2.h"
#include <math.h>
/* sum in the written order (cl would regroup a three term sum) */
#define SUM3(r, a, b, c) do { (r) = (a); (r) = (r) + (b); (r) = (r) + (c); } while (0)
/*0056fb00*/ extern int rwp_math_0056fb00(float *root, float lo, float hi, int degree, float *coef);

/* Quartic root test (ray against a torus-like surface of radii r0, r1): builds the five coefficients of the
   polynomial in the ray parameter and asks rwp_math_0056fb00 for a root in [0,1]. With out == NULL only the
   discriminant-style pre-test (return value) is evaluated. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_grid_00577750(float *out, RwV3d *center, RwV3d *dir, RwV3d *origin, RwV3d *axis, float r0, float r1)
{
  RwV3d d;                /* origin - center; d.x doubles as the root output */
  float c[5];             /* polynomial coefficients */
  float dd, da, rr, r0sq, r1sq, rsum, rdif;
  double dv, ca, sq, q;
  float cd;
  int no_hit;

  RwV3dSubMacro(&d, origin, center);
  SUM3(da, axis->y * d.y, d.x * axis->x, axis->z * d.z);
  SUM3(dd, d.y * d.y, d.x * d.x, d.z * d.z);
  r0sq = r0 * r0;
  r1sq = r1 * r1;
  rsum = r0sq + r1sq;
  rdif = r0sq - r1sq;
  c[0] = dd * dd - (rsum * dd + rsum * dd) + r0sq * da * da * 4.0f + rdif * rdif;
  no_hit = c[0] < 0.0f;
  if (out) {
    if (no_hit) {
      out[0] = 0.0f;
      out[1] = 1.0f;
      return 1;
    }
    SUM3(dv, dir->y * d.y, d.x * dir->x, dir->z * d.z);
    SUM3(ca, dir->y * axis->y, dir->x * axis->x, dir->z * axis->z);
    SUM3(cd, dir->x * dir->x, dir->y * dir->y, dir->z * dir->z);
    q = ca * r0sq;
    q = q * da;
    q = q + q;
    c[1] = ((dd - rsum) * dv + q) * -4.0f;
    sq = dv * dv;
    q = sq + sq;
    q = q + cd * dd;
    q = q + q;
    q = q - (cd * rsum + cd * rsum);
    sq = ca * ca;
    sq = sq * r0sq;
    sq = sq * 4.0f;
    c[2] = q + sq;
    sq = cd * dv;
    c[3] = sq * -4.0f;
    c[4] = cd * cd;
    if (!rwp_math_0056fb00(&d.x, 0.0f, 1.0f, 4, c))
      return 0;
    out[1] = 1.0f;
    out[0] = d.x;
    return 1;
  }
  return no_hit;
}
