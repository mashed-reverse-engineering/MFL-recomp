/* rwp_box_0056d1d0 (0x0056d1d0): RwpBox vtable slot (5f64f0): axis-aligned bounding box of a box */
#include "mfl_tier2.h"

/* Box half sizes at +0x40/+0x44/+0x48, margin at +0x4c.  `frame` (optional) is the box matrix (rows at +0, +0x10, +0x20, position
   +0x30): the world half extent along each axis is sum |R[k][i]| * half[k], grown by the margin.  out[0..2] = max corner,
   out[4..6] = min corner; without a frame the box is centred on the origin. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwp_box_0056d1d0(float *box, float *frame, void *unused, float *out)
{
  float m[3], h[3];
  float sum_x, sum_y, sum_z;

  m[0] = box[19];
  m[1] = box[19];
  m[2] = box[19];
  h[0] = box[16];
  h[1] = box[17];
  h[2] = box[18];
  if (frame != 0) {
    out[0] = (float)fabs(frame[8]) * h[2] + (float)fabs(frame[4]) * h[1] + (float)fabs(frame[0]) * h[0];
    out[1] = (float)fabs(frame[9]) * h[2] + (float)fabs(frame[5]) * h[1] + (float)fabs(frame[1]) * h[0];
    out[2] = (float)fabs(frame[10]) * h[2] + (float)fabs(frame[6]) * h[1] + (float)fabs(frame[2]) * h[0];
    sum_x = m[0] + out[0];
    out[0] = sum_x;
    sum_y = out[1] + m[1];
    out[1] = sum_y;
    sum_z = out[2] + m[2];
    out[2] = sum_z;
    out[4] = frame[12] - sum_x;
    out[5] = frame[13] - sum_y;
    out[6] = frame[14] - sum_z;
    out[0] = frame[12] + sum_x;
    out[1] = frame[13] + sum_y;
    out[2] = frame[14] + sum_z;
  } else {
    sum_x = m[0] + h[0];
    out[0] = sum_x;
    sum_y = h[1] + m[1];
    out[1] = sum_y;
    sum_z = h[2] + m[2];
    out[2] = sum_z;
    out[4] = -sum_x;
    out[5] = -sum_y;
    out[6] = -sum_z;
  }
}
