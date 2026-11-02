/* rwp_cylinder_bounds (0x0056ff10): bounding box of a cylinder placed with a matrix */
#include "mfl_tier2.h"
#include <math.h>

typedef struct MflCylBound {
  unsigned char pad00[0x40];
  float half;   /* 0x40 */
  float radius;   /* 0x44 */
  unsigned char pad48[4];
  float margin;   /* 0x4c */
} MflCylBound;

/* Bounding box of a cylinder (half height, radius, margin) placed with the matrix m (axis row 2 at 8..10, translation at 12..14; m == 0: unplaced):
   half extents e = (r, r, h) unplaced, a sphere bound sqrt(h^2 + r^2) when `exact` is 0, else per axis sqrt(|1 - a^2|) r + |a| h for the z axis a of m; the margin
   is added; out[0..2] = t + e, out[4..6] = t - e.  Returns out. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
float *rwp_cylinder_bounds(MflCylBound *p, float *m, int exact, float *out)
{
  float r = p->radius;
  float h = p->half;
  float mg = p->margin;
  float X, Y, Z, s, ax, ay, az;
  volatile float ex, ey, ez;      /* frame floats in the original: the extents are rounded before the matrix translation is added */
  volatile float zr;

  if (!m) {
    X = r;
    Y = r;
    Z = h;
  } else if (!exact) {
    s = (float)sqrt(h * h + r * r);
    X = s;
    Y = s;
    Z = s;
  } else {
    ax = m[8];
    ay = m[9];
    az = m[10];
    X = (float)sqrt(fabs(1.0f - ax * ax)) * r + (float)fabs(ax) * h;
    Y = (float)sqrt(fabs(1.0f - ay * ay)) * r + (float)fabs(ay) * h;
    zr = (float)sqrt(fabs(1.0f - az * az)) * r + (float)fabs(az) * h;
    Z = zr;
  }
  ex = X + mg;
  ey = Y + mg;
  ez = Z + mg;
  if (m) {
    out[0] = m[12] + ex;
    out[1] = m[13] + ey;
    out[2] = m[14] + ez;
    out[4] = m[12] - ex;
    out[5] = m[13] - ey;
    out[6] = m[14] - ez;
    return out;
  }
  out[4] = -ex;
  out[5] = -ey;
  out[6] = -ez;
  out[0] = ex;
  out[1] = ey;
  out[2] = ez;
  return out;
}
