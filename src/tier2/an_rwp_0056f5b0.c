#include "mfl_tier2.h"
#include <math.h>

/* Same as rwp_math_0056f760 with the matrix transposed (columns 0, 1, 2 of rows 0, 4, 8 swapped for rows): extent of the
   box a2 = (max xyz, min xyz) in the frame of m; out[0..2] = centre + extent, out[4..6] = centre - extent. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_math_0056f5b0(float *out, float *box, float *m)
{
  double acc;
  volatile float hx, hz, cx, cy, cz;
  float ex, ey, dC, wx, wz;
  double hy, ez, dA, dB, wy;

  hx = (box[0] - box[4]) * 0.5f;
  hy = (box[1] - box[5]) * 0.5f;
  hz = (box[2] - box[6]) * 0.5f;
  cx = (box[4] + box[0]) * 0.5f;
  cy = (box[5] + box[1]) * 0.5f;
  cz = (box[6] + box[2]) * 0.5f;
  acc = fabs(m[4]) * hy;
  acc = acc + fabs(m[8]) * hz;
  ex = acc + fabs(m[0]) * hx;
  acc = fabs(m[5]) * hy;
  acc = acc + fabs(m[9]) * hz;
  ey = acc + fabs(m[1]) * hx;
  acc = fabs(m[6]) * hy;
  acc = acc + fabs(m[10]) * hz;
  ez = acc + fabs(m[2]) * hx;
  acc = cy * m[4];
  acc = acc + cz * m[8];
  dA = acc + cx * m[0];
  acc = cy * m[5];
  acc = acc + cz * m[9];
  dB = acc + cx * m[1];
  acc = cy * m[6];
  acc = acc + cz * m[10];
  dC = acc + cx * m[2];
  wx = m[12] + dA;
  wy = m[13] + dB;
  wz = m[14] + dC;
  out[4] = wx - ex;
  out[5] = wy - ey;
  out[6] = wz - ez;
  out[0] = ex + wx;
  out[1] = ey + wy;
  out[2] = ez + wz;
}
