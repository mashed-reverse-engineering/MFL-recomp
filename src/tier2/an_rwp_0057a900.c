#include "mfl_tier2.h"
#include <math.h>

/* Over n float4 triples (a[i], b[i], c[i]): picks the lane (axis 0..3) and entry i whose product a*b exceeds the running bound m,
   or whose c*b is below -m, and raises m to |b| of that lane (so the search ends on the lane with the largest bound).  The entry /
   lane of the last improvement are stored (both -1 when n == 0 or nothing improves). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_batch_0057a900(int *best_index, int *best_axis, float *b, unsigned int n, int a5, int a6, int a7, int a8, int a9, int a10,
                        int a11, int a12, int a13, int a14, float *a, int a16, int a17, float *c, int a19, int a20, float m)
{
  int bi = -1, bx = -1;
  unsigned int i;
  double p0;
  volatile float t[8];   /* p1..p3 and q0..q3: the original rounds them to frame floats (cl kept q0 unrounded on the x87 stack) */
  float *pa = a, *pb = b, *pc = c;

  for (i = 0; i < n; i++, pa += 4, pb += 4, pc += 4) {
    p0 = pa[0] * pb[0];
    t[1] = pa[1] * pb[1];
    t[2] = pa[2] * pb[2];
    t[3] = pa[3] * pb[3];
    t[4] = pc[0] * pb[0];
    t[5] = pc[1] * pb[1];
    t[6] = pc[2] * pb[2];
    t[7] = pc[3] * pb[3];
    if (p0 > m || -m > t[4]) { m = (float)fabs(pb[0]); bi = i; bx = 0; }
    if (t[1] > m || -m > t[5]) { m = (float)fabs(pb[1]); bi = i; bx = 1; }
    if (t[2] > m || -m > t[6]) { m = (float)fabs(pb[2]); bi = i; bx = 2; }
    if (t[3] > m || -m > t[7]) { m = (float)fabs(pb[3]); bi = i; bx = 3; }
  }
  *best_index = bi;
  *best_axis = bx;
}
