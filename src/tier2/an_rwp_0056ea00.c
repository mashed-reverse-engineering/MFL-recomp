#include "mfl_tier2.h"
/* r = a1*a2*a3 + b1*b2*b3 + c1*c2*c3, each product and sum in the written order (cl would regroup a plain expression) */
#define SUMP3(r, a1, a2, a3, b1, b2, b3, c1, c2, c3) do { (r) = (a1) * (a2); (r) = (r) * (a3); t_ = (b1) * (b2); t_ = t_ * (b3); (r) = (r) + t_; t_ = (c1) * (c2); t_ = t_ * (c3); (r) = (r) + t_; } while (0)

/* World-space inertia tensor of a body: dst (3x3, row-major, symmetric) = R * diag(prop[0..2]) * R^T,
   R = rotation matrix of the (conjugated) quaternion prop[3..6] = x, y, z, w.  The quaternion products that
   cl keeps in x87 registers (yw, zw) are doubles, the ones stored to the frame are floats. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_obj_0056ea00(float *dst, float *prop)
{
  double qx = -prop[3], qy = -prop[4], qw = prop[6];
  float qz = -prop[5];
  float xx, yy, zz, zy, zx, yx, xw;
  double yw, zw, acc, t_;
  float r00, r01, r02, r10, r11, r12, r20, r21, r22;

  xx = (float)qx * qx;
  yy = (float)qy * qy;
  zz = qz * qz;
  zy = qz * qy;
  zx = qz * qx;
  yx = (float)qy * qx;
  xw = qx * qw;
  yw = qy * qw;
  zw = qz * qw;
  acc = zz + yy;  acc = acc + acc;  r00 = 1.0f - acc;
  acc = yx + zw;  acc = acc + acc;  r01 = acc;
  acc = zx - yw;  acc = acc + acc;  r02 = acc;
  acc = yx - zw;  acc = acc + acc;  r10 = acc;
  acc = zz + xx;  acc = acc + acc;  r11 = 1.0f - acc;
  acc = xw + zy;  acc = acc + acc;  r12 = acc;
  acc = yw + zx;  acc = acc + acc;  r20 = acc;
  acc = zy - xw;  acc = acc + acc;  r21 = acc;
  acc = yy + xx;  acc = acc + acc;  r22 = 1.0f - acc;
  SUMP3(acc, r00, prop[0], r00, r01, r01, prop[1], r02, prop[2], r02);
  dst[0] = acc;
  SUMP3(acc, r10, prop[0], r00, r11, r01, prop[1], r12, prop[2], r02);
  dst[1] = acc;
  SUMP3(acc, r20, prop[0], r00, r21, r01, prop[1], r22, prop[2], r02);
  dst[2] = acc;
  SUMP3(acc, r10, prop[0], r10, r11, r11, prop[1], r12, prop[2], r12);
  dst[4] = acc;
  SUMP3(acc, r20, prop[0], r10, r21, r11, prop[1], r22, prop[2], r12);
  dst[5] = acc;
  SUMP3(acc, r20, prop[0], r20, r21, r21, prop[1], r22, prop[2], r22);
  dst[8] = acc;
  dst[3] = dst[1];
  dst[6] = dst[2];
  dst[7] = dst[5];
  return *(int *)&dst[7];
}
