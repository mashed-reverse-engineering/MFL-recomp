#include "mfl_tier2.h"

/* Row i of two 8-float-per-row tables: A[0..2] += sum_j B[16j + 0..2] * w[j] and A[4..6] += sum_j B[16j + 4..6] * w[j]
   (j = 0..3; four consecutive rows of B, 0x40 bytes apart).  Partial sums that the original spills to the frame are
   volatile floats, the ones it keeps on the x87 stack are doubles (an unrounded partial sum differs from the spilled one
   in overflow cases). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_batch_0057cf90(int a1, int a2, float *w, int i)
{
  float *A = (float *)(a1 + (i << 5));
  float *B = (float *)(a2 + (i << 5));
  {
    volatile float x1, x2, y2, y3, z3;
    double y1, z1, z2, x3;
    x1 = B[0] * w[0] + A[0];
    y1 = B[1] * w[0] + A[1];
    z1 = B[2] * w[0] + A[2];
    x2 = B[16] * w[1] + x1;
    y2 = B[17] * w[1] + y1;
    z2 = B[18] * w[1] + z1;
    x3 = B[32] * w[2] + x2;
    y3 = B[33] * w[2] + y2;
    z3 = B[34] * w[2] + z2;
    A[0] = B[48] * w[3] + x3;
    A[1] = B[49] * w[3] + y3;
    A[2] = B[50] * w[3] + z3;
  }
  {
    volatile float x1, x2, y2, y3, z3;
    double y1, z1, z2, x3;
    x1 = B[4] * w[0] + A[4];
    y1 = B[5] * w[0] + A[5];
    z1 = B[6] * w[0] + A[6];
    x2 = B[20] * w[1] + x1;
    y2 = B[21] * w[1] + y1;
    z2 = B[22] * w[1] + z1;
    x3 = B[36] * w[2] + x2;
    y3 = B[37] * w[2] + y2;
    z3 = B[38] * w[2] + z2;
    A[4] = B[52] * w[3] + x3;
    A[5] = B[53] * w[3] + y3;
    A[6] = B[54] * w[3] + z3;
  }
}
