#include "mfl_tier2.h"
#include <math.h>

/* RtBaryGetTransform: the barycentric frame of the triangle (corner0, corner1, corner2).  Writes the 4x4 matrix `m` (16 floats; row i = x, y, z of a column):
   column 0 / 1 / 2 hold the barycentric weights of the corners 0 / 1 / 2 as an affine function (m[12..14] their constant parts, the 1 - u - v of
   corner 2 in m[14]), column 3 the unit normal and m[15] its offset to the centroid.  *len receives the area measure |(corner0 - corner1) x (corner1 - corner2)|, or 0
   when it is not larger than 2^-12; the function then returns 0 (the matrix is still written, with a zero normal scale), otherwise 1. */
/* TIER2: unvalidated 2026-11-16 authority=diffemu */
int RtBaryGetTransform(float *m, float *len, float *corner2, float *corner0, float *corner1)
{
  volatile float cen_x, cen_y, cen_z, u0, u1, u2;
  volatile float edge_a_x, edge_a_y, edge_a_z, edge_b_x, edge_b_y, edge_b_z, nrm_x, nrm_y, nrm_z, wgt_x, wgt_y, wgt_z, tx, x2;
  double s, k, ty, tz, n0k, n1k, n2k;
  int ok;
  volatile int iu;

  edge_a_x = corner0[0] - corner1[0];
  edge_a_y = corner0[1] - corner1[1];
  edge_a_z = corner0[2] - corner1[2];
  edge_b_x = corner1[0] - corner2[0];
  edge_b_y = corner1[1] - corner2[1];
  edge_b_z = corner1[2] - corner2[2];
  nrm_x = edge_a_y * edge_b_z - edge_a_z * edge_b_y;
  nrm_y = edge_a_z * edge_b_x - edge_a_x * edge_b_z;
  nrm_z = edge_a_x * edge_b_y - edge_a_y * edge_b_x;
  s = nrm_y * nrm_y;
  s = s + nrm_x * nrm_x;
  s = s + nrm_z * nrm_z;
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
  n0k = nrm_x * k;
  n1k = nrm_y * k;
  n2k = nrm_z * k;
  x2 = (float)(edge_a_x * n1k - edge_a_y * n0k);
  tx = (float)((edge_a_y * n2k - n1k * edge_a_z) * k);
  ty = (n0k * edge_a_z - n2k * edge_a_x) * k;
  tz = x2 * k;
  u0 = n2k * edge_b_y - n1k * edge_b_z;
  iu = *(volatile int *)&u0;
  wgt_x = *(float *)&iu * k;
  u1 = n0k * edge_b_z - n2k * edge_b_x;
  iu = *(volatile int *)&u1;
  wgt_y = *(float *)&iu * k;
  u2 = n1k * edge_b_x - n0k * edge_b_y;
  iu = *(volatile int *)&u2;
  wgt_z = *(float *)&iu * k;
  cen_x = corner2[0] + corner0[0];
  cen_y = corner2[1] + corner0[1];
  cen_z = corner2[2] + corner0[2];
  cen_x = (cen_x + corner1[0]) * 0.3333333432674408f;
  cen_y = corner1[1] + cen_y;
  cen_z = corner1[2] + cen_z;
  cen_y = cen_y * 0.3333333432674408f;
  cen_z = cen_z * 0.3333333432674408f;
  m[0] = tx;
  m[1] = wgt_x;
  m[2] = -(wgt_x + tx);
  m[3] = (float)n0k;
  m[4] = (float)ty;
  m[5] = wgt_y;
  m[6] = (float)-(wgt_y + ty);
  m[7] = (float)n1k;
  m[8] = (float)tz;
  m[9] = wgt_z;
  m[10] = (float)-(wgt_z + tz);
  m[11] = (float)n2k;
  s = corner1[1] * ty;
  s = s + tx * corner1[0];
  s = s + corner1[2] * tz;
  m[12] = (float)-s;
  s = corner2[1] * wgt_y;
  s = s + wgt_x * corner2[0];
  s = s + corner2[2] * wgt_z;
  m[13] = (float)-s;
  m[14] = 1.0f - (m[13] + m[12]);
  s = cen_y * n1k;
  s = s + cen_x * n0k;
  s = s + cen_z * n2k;
  m[15] = (float)-s;
  return ok;
}
