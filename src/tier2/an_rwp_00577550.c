#include "mfl_tier2.h"
#include <math.h>

/* Intersection of the line p + t * dir (p = a4 - a2 is relative to a2) with a sphere / cylinder-like quadric: d = a4 - a2, b = dir . d,
   c = |d|^2 - b^2 - r^2 (q), e = dir . v, g = v . d - e b, h = |v|^2 - e^2, disc = g^2 - h q.
   out[0] = g -/+ sqrt(disc) (the near root, or the far one when flag a7 is set and the near one is behind), out[1] = h.
   Returns 1 with a hit, 0 without, -1 when flag a7 is clear and g <= 0, and the plain inside test (q < 0) when out is NULL.
   (The frame floats b, q, g, disc, u are stored and reloaded by the original; plain floats gave the same results, EQUAL at ext.) */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_grid_00577550(float *out, float *a2, float *v, float *a4, float *dir, float r, int a7, int a8)
{
  float d[3];
  float b, q, g, disc, u;
  double e, f, h, qd, gd, dd;
  int inside;

  d[0] = a4[0] - a2[0];
  d[1] = a4[1] - a2[1];
  d[2] = a4[2] - a2[2];
  qd = dir[1] * d[1];
  qd = qd + d[0] * dir[0];
  qd = qd + dir[2] * d[2];
  b = qd;
  qd = d[1] * d[1];
  qd = qd + d[0] * d[0];
  qd = qd + d[2] * d[2];
  qd = qd - b * b;
  qd = qd - r * r;
  q = qd;
  inside = qd < 0.0f;
  if (out == 0) return inside;
  if (inside && a8 == 0) {
    out[0] = 0.0f;
    out[1] = 1.0f;
    return 1;
  }
  e = dir[1] * v[1];
  e = e + v[0] * dir[0];
  e = e + dir[2] * v[2];
  f = v[1] * d[1];
  f = f + d[0] * v[0];
  f = f + v[2] * d[2];
  gd = e * b;
  gd = f - gd;
  g = gd;
  if (a7 == 0 && g <= 0.0f) return -1;
  h = v[1] * v[1];
  h = h + v[0] * v[0];
  h = h + v[2] * v[2];
  h = h - e * e;
  out[1] = h;
  dd = g * g;
  dd = dd - h * q;
  disc = dd;
  if (dd < 0.0f) return 0;
  u = g - h;
  if (a7 == 0) {
    if (!(u >= 0.0f) || !(u * u >= disc)) {
      out[0] = g - sqrt(disc) * 1.0f;
      return 1;
    }
    return 0;
  }
  if (u >= 0.0f) return 0;
  if (!(u * u <= disc)) {
    out[0] = g - sqrt(disc) * -1.0f;
    return 1;
  }
  return 0;
}
