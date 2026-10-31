#include "mfl_tier2.h"
/* Quaternion q = (x, y, z, w) combined with r: prod = the 16 cross products arranged as a Hamilton-style product
   (4 components) and half = 0.5 * the same sums with the opposite sign pattern (the 12 floats of the second
   product set).  qA_rB is q[A] * r[B]; the ones the original keeps in x87 registers are doubles, the ones it
   spills to the frame are floats; qy_rx_m_qz_rw is volatile because the original rounds it to a frame float
   between two uses (denormal / overflow results differ otherwise). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_constraints_00580550(float *q, float *r, float *prod, float *half)
{
  double acc0, qz_rz;
  double qw_rw, qw_rx, qw_ry, qw_rz, qx_ry, qy_rw, qy_rx;
  float qx_rx, qx_rw, qx_rz, qy_ry, qy_rz, qz_rw, qz_rx, qz_ry, qz_rz_f, qz_rx_qy_rw;
  volatile float qy_rx_m_qz_rw;

  qw_rw = r[3] * q[3];
  qw_rx = r[0] * q[3];
  qw_ry = r[1] * q[3];
  qw_rz = r[2] * q[3];
  qx_rw = q[0] * r[3];
  qx_rx = q[0] * r[0];
  qx_ry = r[1] * q[0];
  qx_rz = r[2] * q[0];
  qy_rw = r[3] * q[1];
  qy_rx = r[0] * q[1];
  qy_ry = r[1] * q[1];
  qy_rz = r[2] * q[1];
  qz_rw = q[2] * r[3];
  qz_rx = q[2] * r[0];
  qz_ry = q[2] * r[1];
  qz_rz = q[2] * r[2];
  qz_rz_f = qz_rz;
  acc0 = qz_rz;
  acc0 = acc0 + qy_ry;
  acc0 = acc0 + qx_rx;
  acc0 = acc0 + qw_rw;
  prod[3] = acc0;
  acc0 = qz_ry;
  acc0 = acc0 - qx_rw;
  acc0 = acc0 + qw_rx;
  acc0 = acc0 - qy_rz;
  prod[0] = acc0;
  qz_rx_qy_rw = qz_rx + qy_rw;
  acc0 = qx_rz;
  acc0 = acc0 - qz_rx_qy_rw;
  acc0 = acc0 + qw_ry;
  prod[1] = acc0;
  acc0 = qy_rx;
  acc0 = acc0 - qz_rw;
  qy_rx_m_qz_rw = acc0;
  acc0 = acc0 - qx_ry;
  acc0 = acc0 + qw_rz;
  prod[2] = acc0;
  acc0 = qx_rw;
  acc0 = acc0 - qw_rx;
  acc0 = acc0 - qy_rz;
  acc0 = acc0 + qz_ry;
  half[0] = acc0 * 0.5f;
  acc0 = qy_rw;
  acc0 = acc0 - qw_ry;
  acc0 = acc0 + qx_rz;
  acc0 = acc0 - qz_rx;
  half[1] = acc0 * 0.5f;
  acc0 = qz_rw;
  acc0 = acc0 - (qx_ry + qw_rz);
  acc0 = acc0 + qy_rx;
  half[2] = acc0 * 0.5f;
  acc0 = qx_rx;
  acc0 = acc0 + qw_rw;
  acc0 = acc0 - qz_rz_f;
  acc0 = acc0 - qy_ry;
  half[3] = acc0 * 0.5f;
  acc0 = qz_rw;
  acc0 = acc0 + qy_rx;
  acc0 = acc0 + qx_ry;
  acc0 = acc0 + qw_rz;
  half[4] = acc0 * 0.5f;
  acc0 = qx_rz;
  acc0 = acc0 - qw_ry;
  acc0 = acc0 + qz_rx;
  acc0 = acc0 - qy_rw;
  half[5] = acc0 * 0.5f;
  acc0 = (float)qy_rx_m_qz_rw;
  acc0 = acc0 - qw_rz;
  acc0 = acc0 + qx_ry;
  half[6] = acc0 * 0.5f;
  acc0 = qy_ry;
  acc0 = acc0 - qz_rz_f;
  acc0 = acc0 + qw_rw;
  acc0 = acc0 - qx_rx;
  half[7] = acc0 * 0.5f;
  qy_rx_m_qz_rw = qz_ry + qy_rz;
  acc0 = qz_ry;
  acc0 = acc0 + qy_rz;
  acc0 = acc0 + qx_rw;
  acc0 = acc0 + qw_rx;
  half[8] = acc0 * 0.5f;
  acc0 = qz_rx_qy_rw;
  acc0 = acc0 + qx_rz;
  acc0 = acc0 + qw_ry;
  half[9] = acc0 * 0.5f;
  acc0 = (float)qy_rx_m_qz_rw;
  acc0 = acc0 - qx_rw;
  acc0 = acc0 - qw_rx;
  half[10] = acc0 * 0.5f;
  acc0 = qz_rz_f;
  acc0 = acc0 - qy_ry;
  acc0 = acc0 - qx_rx;
  acc0 = acc0 + qw_rw;
  half[11] = acc0 * 0.5f;
}
