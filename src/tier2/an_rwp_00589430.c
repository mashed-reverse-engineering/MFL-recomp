#include "mfl_tier2.h"
#include <math.h>

extern float rwp_cylinder_0056fd20(float *out, float *in);
extern int rwp_cylinder_0056fd90(float *out, float *v);

/* Direction from b to a projected onto the plane with normal n: d = a - b, d += n * -(n . d), then normalised.  When the projection
   is tiny against the removed part (|t| > FLT_MIN and length / |t| <= 0.001) or the removed part is tiny, d is replaced by an
   arbitrary unit vector perpendicular to n (rwp_cylinder_0056fd90).  Result in d. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_aggregate_00589430(float *a, float *b, float *n, float *d)
{
  volatile float t;      /* the original rounds t to a frame float (volatile keeps the rounding) */
  double len, dz;
  volatile float *vd = d;      /* reload d[2] from memory (the original does not reuse the unrounded register value) */

  d[0] = a[0] - b[0];
  d[1] = a[1] - b[1];
  dz = a[2] - b[2];
  d[2] = dz;
  t = -(n[1] * d[1] + n[0] * d[0] + n[2] * dz);
  (void)t;      /* reload: the original reads t back from its frame slot also for the first term */
  d[0] = t * n[0] + d[0];
  d[1] = n[1] * t + d[1];
  vd[2] = n[2] * t + vd[2];
  len = rwp_cylinder_0056fd20(d, d);
  if (fabs(t) > 1.1754944e-38f) {
    if (len / fabs(t) > 0.001f) return;
  }
  rwp_cylinder_0056fd90(d, n);
  rwp_cylinder_0056fd20(d, d);
}
