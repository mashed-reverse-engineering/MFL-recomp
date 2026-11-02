#include "mfl_tier2.h"
#include <math.h>

/* RtBaryGetTransform: the barycentric frame of the triangle (p0, p1, p2).  Writes the 4x4 matrix `m` (16 floats; row i = x, y, z of a column):
   column 0 / 1 / 2 hold the barycentric weights of the corners 0 / 1 / 2 as an affine function (m[12..14] their constant parts, the 1 - u - v of
   corner 2 in m[14]), column 3 the unit normal and m[15] its offset to the centroid.  *len receives the area measure |(p0 - p1) x (p1 - p2)|, or 0
   when it is not larger than 2^-12; the function then returns 0 (the matrix is still written, with a zero normal scale), otherwise 1. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
int RtBaryGetTransform(float *m, float *len, float *p2, float *p0, float *p1)
{
  volatile float g0, g1, g2, u0, u1, u2;
  volatile float a0, a1, a2, b0, b1, b2, n0, n1, n2, w0, w1, w2, tx, x2;
  double s, k, ty, tz, n0k, n1k, n2k;
  int ok;
  volatile int iu;

  a0 = p0[0] - p1[0];
  a1 = p0[1] - p1[1];
  a2 = p0[2] - p1[2];
  b0 = p1[0] - p2[0];
  b1 = p1[1] - p2[1];
  b2 = p1[2] - p2[2];
  n0 = a1 * b2 - a2 * b1;
  n1 = a2 * b0 - a0 * b2;
  n2 = a0 * b1 - a1 * b0;
  s = n1 * n1;
  s = s + n0 * n0;
  s = s + n2 * n2;
  if (s > 0.000244140625) {
    ok = 1;
    k = sqrt(s);
    *len = (float)k;
    k = 1.0 / k;
  } else {
    ok = 0;
    *(int *)len = 0;
    k = 0.0;
  }
  n0k = n0 * k;
  n1k = n1 * k;
  n2k = n2 * k;
  x2 = (float)(a0 * n1k - a1 * n0k);
  tx = (float)((a1 * n2k - n1k * a2) * k);
  ty = (n0k * a2 - n2k * a0) * k;
  tz = x2 * k;
  u0 = n2k * b1 - n1k * b2;
  iu = *(volatile int *)&u0;
  w0 = *(float *)&iu * k;
  u1 = n0k * b2 - n2k * b0;
  iu = *(volatile int *)&u1;
  w1 = *(float *)&iu * k;
  u2 = n1k * b0 - n0k * b1;
  iu = *(volatile int *)&u2;
  w2 = *(float *)&iu * k;
  g0 = p2[0] + p0[0];
  g1 = p2[1] + p0[1];
  g2 = p2[2] + p0[2];
  g0 = (g0 + p1[0]) * 0.3333333432674408f;
  g1 = p1[1] + g1;
  g2 = p1[2] + g2;
  g1 = g1 * 0.3333333432674408f;
  g2 = g2 * 0.3333333432674408f;
  m[0] = tx;
  m[1] = w0;
  m[2] = -(w0 + tx);
  m[3] = (float)n0k;
  m[4] = (float)ty;
  m[5] = w1;
  m[6] = (float)-(w1 + ty);
  m[7] = (float)n1k;
  m[8] = (float)tz;
  m[9] = w2;
  m[10] = (float)-(w2 + tz);
  m[11] = (float)n2k;
  s = p1[1] * ty;
  s = s + tx * p1[0];
  s = s + p1[2] * tz;
  m[12] = (float)-s;
  s = p2[1] * w1;
  s = s + w0 * p2[0];
  s = s + p2[2] * w2;
  m[13] = (float)-s;
  m[14] = 1.0f - (m[13] + m[12]);
  s = g1 * n1k;
  s = s + g0 * n0k;
  s = s + g2 * n2k;
  m[15] = (float)-s;
  return ok;
}
