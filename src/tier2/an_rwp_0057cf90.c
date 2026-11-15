/* rwp_rows_add_weighted (0x0057cf90): add weighted rows of one 8-float-per-row table into another */
#include "mfl_tier2.h"

/* Row i of two 8-float-per-row tables: A[0..2] += sum_j B[16j + 0..2] * w[j] and A[4..6] += sum_j B[16j + 4..6] * w[j]
   (j = 0..3; four consecutive rows of B, 0x40 bytes apart).  Partial sums that the original spills to the frame are
   volatile floats, the ones it keeps on the x87 stack are doubles (an unrounded partial sum differs from the spilled one
   in overflow cases). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_rows_add_weighted(int dst_addr, int src_addr, float *w, int i)
{
  float *A = (float *)(dst_addr + (i << 5));
  float *B = (float *)(src_addr + (i << 5));
  {
    volatile float sum_x1, sum_x2, sum_y2, sum_y3, sum_z3;
    double sum_y1, sum_z1, sum_z2, sum_x3;
    sum_x1 = B[0] * w[0] + A[0];
    sum_y1 = B[1] * w[0] + A[1];
    sum_z1 = B[2] * w[0] + A[2];
    sum_x2 = B[16] * w[1] + sum_x1;
    sum_y2 = B[17] * w[1] + sum_y1;
    sum_z2 = B[18] * w[1] + sum_z1;
    sum_x3 = B[32] * w[2] + sum_x2;
    sum_y3 = B[33] * w[2] + sum_y2;
    sum_z3 = B[34] * w[2] + sum_z2;
    A[0] = B[48] * w[3] + sum_x3;
    A[1] = B[49] * w[3] + sum_y3;
    A[2] = B[50] * w[3] + sum_z3;
  }
  {
    volatile float sum_x1, sum_x2, sum_y2, sum_y3, sum_z3;
    double sum_y1, sum_z1, sum_z2, sum_x3;
    sum_x1 = B[4] * w[0] + A[4];
    sum_y1 = B[5] * w[0] + A[5];
    sum_z1 = B[6] * w[0] + A[6];
    sum_x2 = B[20] * w[1] + sum_x1;
    sum_y2 = B[21] * w[1] + sum_y1;
    sum_z2 = B[22] * w[1] + sum_z1;
    sum_x3 = B[36] * w[2] + sum_x2;
    sum_y3 = B[37] * w[2] + sum_y2;
    sum_z3 = B[38] * w[2] + sum_z2;
    A[4] = B[52] * w[3] + sum_x3;
    A[5] = B[53] * w[3] + sum_y3;
    A[6] = B[54] * w[3] + sum_z3;
  }
}
