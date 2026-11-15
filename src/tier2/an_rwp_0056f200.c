/* rwp_triangle_incircle (0x0056f200): incentre and inradius of a triangle */
#include "mfl_tier2.h"
#include <math.h>

/* Incentre and inradius of the triangle pt_a, pt_b, pt_c: out[0..2] = (|a3a4| pt_a + |a4a2| pt_b + |a2a3| pt_c) / perimeter,
   out[3] = area / half perimeter (Heron).  Returns the longest side.  The side lengths of a4a2 and a2a3 and the area are
   rounded to frame floats (volatile); the first side stays a double on the x87 stack. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float rwp_triangle_incircle(float *out, float *pt_a, float *pt_b, float *pt_c)
{
  double acc, s, guard, r, k, len_bc, len_ab_d;
  volatile float len_ca, len_ab, area;
  float dx, dy, dz;

  dz = pt_b[2] - pt_c[2];
  dy = pt_b[1] - pt_c[1];
  dx = pt_b[0] - pt_c[0];
  acc = dx * dx;
  acc = acc + dy * dy;
  acc = acc + dz * dz;
  len_bc = sqrt(acc);
  dz = pt_c[2] - pt_a[2];
  dy = pt_c[1] - pt_a[1];
  dx = pt_c[0] - pt_a[0];
  acc = dx * dx;
  acc = acc + dy * dy;
  acc = acc + dz * dz;
  len_ca = sqrt(acc);
  dz = pt_a[2] - pt_b[2];
  dy = pt_a[1] - pt_b[1];
  dx = pt_a[0] - pt_b[0];
  acc = dx * dx;
  acc = acc + dy * dy;
  acc = acc + dz * dz;
  len_ab_d = sqrt(acc);
  len_ab = len_ab_d;
  acc = len_ab_d;
  acc = acc + len_ca;
  acc = acc + len_bc;
  s = acc * 0.5f;
  acc = s - len_ab;
  acc = acc * (s - len_ca);
  acc = acc * (s - len_bc);
  acc = acc * s;
  area = sqrt(acc);
  if (s < 1.1754944e-38f)
    guard = 1.0f;
  else
    guard = s;
  r = 1.0f / guard;
  out[3] = area * r;
  out[0] = len_bc * pt_a[0];
  out[1] = pt_a[1] * len_bc;
  out[2] = pt_a[2] * len_bc;
  out[0] = len_ca * pt_b[0] + out[0];
  out[1] = pt_b[1] * len_ca + out[1];
  out[2] = pt_b[2] * len_ca + out[2];
  out[0] = len_ab * pt_c[0] + out[0];
  out[1] = pt_c[1] * len_ab + out[1];
  out[2] = pt_c[2] * len_ab + out[2];
  k = r * 0.5f;
  out[0] = k * out[0];
  out[1] = out[1] * k;
  out[2] = out[2] * k;
  return len_bc >= (len_ca >= len_ab ? len_ca : len_ab) ? len_bc : (len_ca >= len_ab ? len_ca : len_ab);
}
