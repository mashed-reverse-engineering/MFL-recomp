#include "mfl_tier2.h"
#include <math.h>
#pragma intrinsic(sin, cos)

/*004d5410*/ extern float _rwSqrt(float p0);

/* Rotates the quaternion q = (x, y, z, w) by the axis-angle rotation (axis v, angle = |v| * angle * 0.5 as half angle): q = q (cos h, sin h * n) with
   n = v / |v|, skipped when |v| < 1e-4.  The values the original keeps on the x87 stack are doubles (nz, the half angle); the frame floats nx, ny, sin
   live in k[], cos and the cross term (k4) are volatile (cl would keep them in a register unrounded; cs2 = cs makes it reload cos like the original). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_batch_0057c230(float *q, float *v, float angle)
{
  float k[3], q0, q1, q2;
  volatile float cs, cs2, k4;
  float len;
  double len2, inv, nz, h, t, cr, w;

  len2 = v[1] * v[1];
  len2 = len2 + v[0] * v[0];
  len2 = len2 + v[2] * v[2];
  len = _rwSqrt(len2);
  if (!(fabs(len) < 9.99999974737875e-5f)) {
    inv = 1.0f / len;
    q0 = q[0];
    q1 = q[1];
    q2 = q[2];
    k[0] = inv * v[0];
    k[1] = inv * v[1];
    nz = inv * v[2];
    h = len * angle;
    h = h * 0.5f;
    k[2] = sin(h);
    cs = cos(h);
    cs2 = cs;
    w = cs * q[3];
    t = q1 * k[1];
    t = t + q0 * k[0];
    t = t + q2 * nz;
    t = t * k[2];
    w = w - t;
    cr = q2 * k[1];
    cr = cr - q1 * nz;
    k4 = q1 * k[0] - q0 * k[1];
    t = k[0] * q[3] + cr;
    t = t * k[2];
    t = t + q0 * cs;
    q[0] = t;
    t = k[1] * q[3] + ((q0 * nz) - (q2 * k[0]));
    t = t * k[2];
    t = t + q1 * cs;
    q[1] = t;
    t = nz * q[3] + k4;
    t = t * k[2];
    t = t + q2 * cs;
    q[2] = t;
    q[3] = w;
  }
}
