#include "mfl_tier2.h"

/* The 0x88 byte argument block (34 dwords) that the row solver functions take by value and pass on unchanged. */
typedef struct MflSolveByVal34 {
  int w[34];
} MflSolveByVal34;

extern void rwp_batch_0057ae50(float *acc, int a4, int a5, MflSolveByVal34 blk, float delta, int idx);

/* One clamped row update of the solver: lambda = clamp(hi - lo, min, max) is stored in lam[idx] (idx = *row + 4 * *col), the two flag arrays
   get 0 when lambda sits on the lower / upper limit and -1 otherwise, the row solver 57ae50 is run with the change (lambda - hi) and the
   accumulator entry acc[idx] gets (scale * weight + bias) * change added. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_batch_0057b190(int *col, int *row, float *acc, int a4, int a5, int *at_lo, int a7, int a8, int *at_hi, int a10,
                        int a11, float *lam, int a13, int a14, MflSolveByVal34 blk, float lo, float lim_min, float lim_max,
                        float hi, float scale, float bias, float weight)
{
  float d = hi - lo;
  int idx;

  if (d < lim_min) d = lim_min;
  if (lim_max < d) d = lim_max;
  lam[*row + *col * 4] = d;
  if (d == lim_min) at_lo[*row + *col * 4] = 0; else at_lo[*row + *col * 4] = -1;
  if (d == lim_max) at_hi[*row + *col * 4] = 0; else at_hi[*row + *col * 4] = -1;
  lim_min = d - hi;
  idx = *row + *col * 4;
  rwp_batch_0057ae50(acc, a4, a5, blk, lim_min, idx);
  acc[*row + *col * 4] += (scale * weight + bias) * lim_min;
}
