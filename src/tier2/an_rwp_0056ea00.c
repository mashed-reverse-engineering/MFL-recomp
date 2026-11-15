/* rwp_inertia_world_tensor (0x0056ea00): world-space inertia tensor R * diag * R^T of a body */
#include "mfl_tier2.h"
/* r = a1*a2*a3 + b1*b2*b3 + c1*c2*c3, each product and sum in the written order (cl would regroup a plain expression) */
#define SUMP3(r, a1, a2, a3, b1, b2, b3, c1, c2, c3) do { (r) = (a1) * (a2); (r) = (r) * (a3); t_ = (b1) * (b2); t_ = t_ * (b3); (r) = (r) + t_; t_ = (c1) * (c2); t_ = t_ * (c3); (r) = (r) + t_; } while (0)

/* World-space inertia tensor of a body: dst (3x3, row-major, symmetric) = R * diag(prop[0..2]) * R^T,
   R = rotation matrix of the (conjugated) quaternion prop[3..6] = x, y, z, w.  The quaternion products that
   cl keeps in x87 registers (yw, zw) are doubles, the ones stored to the frame are floats. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_inertia_world_tensor(float *dst, float *prop)
{
  double qx = -prop[3], qy = -prop[4], qw = prop[6];
  float qz = -prop[5];
  float xx, yy, zz, zy, zx, yx, xw;
  double yw, zw, acc, t_;
  float rot_xx, rot_xy, rot_xz, rot_yx, rot_yy, rot_yz, rot_zx, rot_zy, rot_zz;

  xx = (float)qx * qx;
  yy = (float)qy * qy;
  zz = qz * qz;
  zy = qz * qy;
  zx = qz * qx;
  yx = (float)qy * qx;
  xw = qx * qw;
  yw = qy * qw;
  zw = qz * qw;
  acc = zz + yy;  acc = acc + acc;  rot_xx = 1.0f - acc;
  acc = yx + zw;  acc = acc + acc;  rot_xy = acc;
  acc = zx - yw;  acc = acc + acc;  rot_xz = acc;
  acc = yx - zw;  acc = acc + acc;  rot_yx = acc;
  acc = zz + xx;  acc = acc + acc;  rot_yy = 1.0f - acc;
  acc = xw + zy;  acc = acc + acc;  rot_yz = acc;
  acc = yw + zx;  acc = acc + acc;  rot_zx = acc;
  acc = zy - xw;  acc = acc + acc;  rot_zy = acc;
  acc = yy + xx;  acc = acc + acc;  rot_zz = 1.0f - acc;
  SUMP3(acc, rot_xx, prop[0], rot_xx, rot_xy, rot_xy, prop[1], rot_xz, prop[2], rot_xz);
  dst[0] = acc;
  SUMP3(acc, rot_yx, prop[0], rot_xx, rot_yy, rot_xy, prop[1], rot_yz, prop[2], rot_xz);
  dst[1] = acc;
  SUMP3(acc, rot_zx, prop[0], rot_xx, rot_zy, rot_xy, prop[1], rot_zz, prop[2], rot_xz);
  dst[2] = acc;
  SUMP3(acc, rot_yx, prop[0], rot_yx, rot_yy, rot_yy, prop[1], rot_yz, prop[2], rot_yz);
  dst[4] = acc;
  SUMP3(acc, rot_zx, prop[0], rot_yx, rot_zy, rot_yy, prop[1], rot_zz, prop[2], rot_yz);
  dst[5] = acc;
  SUMP3(acc, rot_zx, prop[0], rot_zx, rot_zy, rot_zy, prop[1], rot_zz, prop[2], rot_zz);
  dst[8] = acc;
  dst[3] = dst[1];
  dst[6] = dst[2];
  dst[7] = dst[5];
  return *(int *)&dst[7];
}
