#include "mfl_tier2.h"
#include <math.h>

/* Integrates the orientation quaternion q = (x, y, z, w) by the angular velocity omega over dt:
   q += 0.5 * dt * (omega * q), then renormalises.  The sums keep the original order; the new w
   (q_w_new) is volatile: the original rounds it to a frame float that is read twice (norm, final scale),
   so overflow / denormal results differ in a register. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_batch_0057c390(float *q, float *omega, float dt)
{
  double acc, dw, dx;
  float inv_len;
  float d[2];      /* frame floats (an array keeps them in memory): dy, dz */
  volatile float q_w_new;
  float q_z_new;
  double q_x_new, q_y_new;

  acc = -(q[0] * omega[0]);
  acc = acc - omega[1] * q[1];
  acc = acc - omega[2] * q[2];
  dw = acc * 0.5f;
  acc = omega[1] * q[2];
  acc = acc + q[3] * omega[0];
  acc = acc - omega[2] * q[1];
  dx = acc * 0.5f;
  acc = omega[1] * q[3];
  acc = acc - q[2] * omega[0];
  acc = acc + omega[2] * q[0];
  d[0] = acc * 0.5f;
  acc = q[1] * omega[0];
  acc = acc - omega[1] * q[0];
  acc = acc + omega[2] * q[3];
  d[1] = acc * 0.5f;
  q_w_new = dw * dt + q[3];
  q_x_new = dx * dt + q[0];
  q_y_new = d[0] * dt + q[1];
  q_z_new = d[1] * dt + q[2];
  acc = q_x_new * q_x_new;
  acc = acc + q_y_new * q_y_new;
  acc = acc + q_z_new * q_z_new;
  acc = acc + q_w_new * q_w_new;
  inv_len = 1.0f / sqrt(acc);
  q[3] = q_w_new * inv_len;
  q[0] = q_x_new * inv_len;
  q[1] = q_y_new * inv_len;
  q[2] = q_z_new * inv_len;
}
