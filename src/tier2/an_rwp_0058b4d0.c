#include "mfl_tier2.h"
#include <math.h>

/* Bounding sphere of a triangle s (three points of 4 floats: x, y, z, pad; the radius slot at s[19] holds an extra margin):
   the centroid goes to out[0..2], out[3] = distance of the vertex selected by the ordering of the three squared distances
   (table lookup) plus the margin.  Returns out. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float *rwp_triangle_0058b4d0(float *s, float *out)
{
  double o0, o1, o2, t0, t1, t2, acc;
  float d[3], t2c;
  int c01, c02, c12, sel;

  out[0] = s[4] + s[0];
  out[1] = s[5] + s[1];
  out[2] = s[6] + s[2];
  out[0] = s[8] + out[0];
  out[1] = s[9] + out[1];
  out[2] = s[10] + out[2];
  o0 = out[0] * 0.33333334f;
  out[0] = o0;
  o1 = out[1] * 0.33333334f;
  out[1] = o1;
  o2 = out[2] * 0.33333334f;
  out[2] = o2;
  t2 = o2 - s[2];
  t1 = o1 - s[1];
  t0 = o0 - s[0];
  acc = t0 * t0;
  acc = acc + t1 * t1;
  acc = acc + t2 * t2;
  d[0] = -acc;
  t2 = o2 - s[6];
  t1 = o1 - s[5];
  t0 = o0 - s[4];
  acc = t0 * t0;
  acc = acc + t1 * t1;
  acc = acc + t2 * t2;
  d[1] = -acc;
  t2c = o2 - s[10];
  t1 = o1 - s[9];
  t0 = o0 - s[8];
  acc = t0 * t0;
  acc = acc + t1 * t1;
  acc = acc + t2c * t2c;
  d[2] = -acc;
  c01 = d[0] > d[1];
  c02 = d[0] > d[2];
  c12 = d[1] > d[2];
  sel = (0x21312300 >> ((((c01 << 1) | c02) << 1 | c12) << 2)) & 3;
  out[3] = sqrt(-d[sel]) + s[19];
  return out;
}
