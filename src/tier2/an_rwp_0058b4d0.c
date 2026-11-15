/* rwp_triangle_bounding_sphere (0x0058b4d0): bounding sphere of a triangle */
#include "mfl_tier2.h"
#include <math.h>

/* Bounding sphere of a triangle s (three points of 4 floats: x, y, z, pad; the radius slot at s[19] holds an extra margin):
   the centroid goes to out[0..2], out[3] = distance of the vertex selected by the ordering of the three squared distances
   (table lookup) plus the margin.  Returns out. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float *rwp_triangle_bounding_sphere(float *s, float *out)
{
  double ctr_x, ctr_y, ctr_z, dx, dy, dz, acc;
  float d[3], dz_c;
  int cmp_01, cmp_02, cmp_12, sel;

  out[0] = s[4] + s[0];
  out[1] = s[5] + s[1];
  out[2] = s[6] + s[2];
  out[0] = s[8] + out[0];
  out[1] = s[9] + out[1];
  out[2] = s[10] + out[2];
  ctr_x = out[0] * 0.33333334f;
  out[0] = ctr_x;
  ctr_y = out[1] * 0.33333334f;
  out[1] = ctr_y;
  ctr_z = out[2] * 0.33333334f;
  out[2] = ctr_z;
  dz = ctr_z - s[2];
  dy = ctr_y - s[1];
  dx = ctr_x - s[0];
  acc = dx * dx;
  acc = acc + dy * dy;
  acc = acc + dz * dz;
  d[0] = -acc;
  dz = ctr_z - s[6];
  dy = ctr_y - s[5];
  dx = ctr_x - s[4];
  acc = dx * dx;
  acc = acc + dy * dy;
  acc = acc + dz * dz;
  d[1] = -acc;
  dz_c = ctr_z - s[10];
  dy = ctr_y - s[9];
  dx = ctr_x - s[8];
  acc = dx * dx;
  acc = acc + dy * dy;
  acc = acc + dz_c * dz_c;
  d[2] = -acc;
  cmp_01 = d[0] > d[1];
  cmp_02 = d[0] > d[2];
  cmp_12 = d[1] > d[2];
  sel = (0x21312300 >> ((((cmp_01 << 1) | cmp_02) << 1 | cmp_12) << 2)) & 3;
  out[3] = sqrt(-d[sel]) + s[19];
  return out;
}
