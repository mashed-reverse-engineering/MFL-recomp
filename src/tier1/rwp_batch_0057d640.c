/* compile: /G5 */
#include "mfl_tier2.h"
/* Gathers six columns of four rows (row stride 16 words, pair index n) of an interleaved matrix block into a contiguous 24 word record. (Rwp library) */
void rwp_batch_0057d640(unsigned *out, unsigned *m, int n)
{
  unsigned *col = m + n * 8;
  out[0] = m[n * 8];
  out[1] = m[(n + 2) * 8];
  out[2] = m[(n + 4) * 8];
  out[3] = m[(n + 6) * 8];
  out[4] = col[1];
  out[5] = col[17];
  out[6] = col[33];
  out[7] = col[49];
  out[8] = col[2];
  out[9] = col[18];
  out[10] = col[34];
  out[11] = col[50];
  out[12] = col[4];
  out[13] = col[20];
  out[14] = col[36];
  out[15] = col[52];
  out[16] = col[5];
  out[17] = col[21];
  out[18] = col[37];
  out[19] = col[53];
  out[20] = col[6];
  out[21] = col[22];
  out[22] = col[38];
  out[23] = col[54];
}
