#include "mfl_tier2.h"
#include <math.h>

/* Oriented box a2 = (max xyz, min xyz) against the axes of the matrix m (rows 0, 4, 8; translation at 12..14):
   out[0..2] = centre (in m's frame) + extent, out[4..6] = centre - extent, i.e. the extent of the box along m's axes.
   Half sizes / centre are rounded to frame floats like the original (volatile: overflow / denormal results differ
   in a register), the y half size and the sums kept on the x87 stack are doubles. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_math_0056f760(float *out, float *box, float *m)
{
  double acc;
  volatile float hx, hz, cx, cy, cz; float ex, ey, d0, d1;
  double hy, ez, dx, dy, dz, d2;

  hx = (box[0] - box[4]) * 0.5f;
  hy = (box[1] - box[5]) * 0.5f;
  hz = (box[2] - box[6]) * 0.5f;
  cx = (box[4] + box[0]) * 0.5f;
  cy = (box[5] + box[1]) * 0.5f;
  cz = (box[6] + box[2]) * 0.5f;
  acc = fabs(m[1]) * hy;
  acc = acc + fabs(m[2]) * hz;
  ex = acc + fabs(m[0]) * hx;
  acc = fabs(m[5]) * hy;
  acc = acc + fabs(m[6]) * hz;
  ey = acc + fabs(m[4]) * hx;
  acc = fabs(m[9]) * hy;
  acc = acc + fabs(m[10]) * hz;
  ez = acc + fabs(m[8]) * hx;
  dx = cx - m[12];
  dy = cy - m[13];
  dz = cz - m[14];
  acc = m[1] * dy;
  acc = acc + dx * m[0];
  d0 = acc + m[2] * dz;
  acc = m[5] * dy;
  acc = acc + m[4] * dx;
  d1 = acc + m[6] * dz;
  acc = m[9] * dy;
  acc = acc + m[8] * dx;
  d2 = acc + m[10] * dz;
  out[4] = d0 - ex;
  out[5] = d1 - ey;
  out[6] = d2 - ez;
  out[0] = ex + d0;
  out[1] = ey + d1;
  out[2] = ez + d2;
}
