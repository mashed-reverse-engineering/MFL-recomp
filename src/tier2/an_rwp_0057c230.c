/* rwp_quat_rotate_axis_angle (0x0057c230): rotate a quaternion by an axis-angle rotation */
#include "mfl_tier2.h"
#include <math.h>
#pragma intrinsic(sin, cos)

/*004d5410*/ extern float _rwSqrt(float value);

/* Rotates the quaternion q = (x, y, z, w) by the axis-angle rotation (axis v, angle = |v| * angle * 0.5 as half angle): q = q (cos h, sin h * n) with
   n = v / |v|, skipped when |v| < 1e-4.  The values the original keeps on the x87 stack are doubles (nz, the half angle); the frame floats nx, ny, sin
   live in k[], cos and the cross term (cross_z) are volatile (cl would keep them in a register unrounded; cs2 = cs makes it reload cos like the original). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_quat_rotate_axis_angle(float *q, float *v, float angle)
{
  float k[3], qx, qy, qz;
  volatile float cs, cs2, cross_z;
  float len;
  double len2, inv, nz, h, t, cr, w;

  len2 = v[1] * v[1];
  len2 = len2 + v[0] * v[0];
  len2 = len2 + v[2] * v[2];
  len = _rwSqrt(len2);
  if (!(fabs(len) < 9.99999974737875e-5f)) {
    inv = 1.0f / len;
    qx = q[0];
    qy = q[1];
    qz = q[2];
    k[0] = inv * v[0];
    k[1] = inv * v[1];
    nz = inv * v[2];
    h = len * angle;
    h = h * 0.5f;
    k[2] = sin(h);
    cs = cos(h);
    cs2 = cs;
    w = cs * q[3];
    t = qy * k[1];
    t = t + qx * k[0];
    t = t + qz * nz;
    t = t * k[2];
    w = w - t;
    cr = qz * k[1];
    cr = cr - qy * nz;
    cross_z = qy * k[0] - qx * k[1];
    t = k[0] * q[3] + cr;
    t = t * k[2];
    t = t + qx * cs;
    q[0] = t;
    t = k[1] * q[3] + ((qx * nz) - (qz * k[0]));
    t = t * k[2];
    t = t + qy * cs;
    q[1] = t;
    t = nz * q[3] + cross_z;
    t = t * k[2];
    t = t + qz * cs;
    q[2] = t;
    q[3] = w;
  }
}
