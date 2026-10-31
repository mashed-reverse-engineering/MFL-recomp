#include "mfl_tier2.h"
#include <math.h>

/* Incentre and inradius of the triangle a2, a3, a4: out[0..2] = (|a3a4| a2 + |a4a2| a3 + |a2a3| a4) / perimeter,
   out[3] = area / half perimeter (Heron).  Returns the longest side.  The side lengths of a4a2 and a2a3 and the area are
   rounded to frame floats (volatile); the first side stays a double on the x87 stack. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float rwp_math_0056f200(float *out, float *a2, float *a3, float *a4)
{
  double acc, s, guard, r, k, len_bc, len_ab_d;
  volatile float len_ca, len_ab, area;
  float dx, dy, dz;

  dz = a3[2] - a4[2];
  dy = a3[1] - a4[1];
  dx = a3[0] - a4[0];
  acc = dx * dx;
  acc = acc + dy * dy;
  acc = acc + dz * dz;
  len_bc = sqrt(acc);
  dz = a4[2] - a2[2];
  dy = a4[1] - a2[1];
  dx = a4[0] - a2[0];
  acc = dx * dx;
  acc = acc + dy * dy;
  acc = acc + dz * dz;
  len_ca = sqrt(acc);
  dz = a2[2] - a3[2];
  dy = a2[1] - a3[1];
  dx = a2[0] - a3[0];
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
  out[0] = len_bc * a2[0];
  out[1] = a2[1] * len_bc;
  out[2] = a2[2] * len_bc;
  out[0] = len_ca * a3[0] + out[0];
  out[1] = a3[1] * len_ca + out[1];
  out[2] = a3[2] * len_ca + out[2];
  out[0] = len_ab * a4[0] + out[0];
  out[1] = a4[1] * len_ab + out[1];
  out[2] = a4[2] * len_ab + out[2];
  k = r * 0.5f;
  out[0] = k * out[0];
  out[1] = out[1] * k;
  out[2] = out[2] * k;
  return len_bc >= (len_ca >= len_ab ? len_ca : len_ab) ? len_bc : (len_ca >= len_ab ? len_ca : len_ab);
}
