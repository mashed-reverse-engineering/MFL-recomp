#include "mfl_tier2.h"

/* One root of the polynomial poly (degree n, coefficients ascending) inside the bracket [x0, x1] with f0 = P(x0), f1 = P(x1) of opposite sign
   (dpoly = its derivative): Newton steps from the midpoint that are safeguarded by the bracket (a step that leaves it is replaced by bisection),
   at most 23 iterations; stops when the step is below about 1.2e-7 of the value.  Returns the root (the midpoint when the bracket is invalid or the
   function is exactly zero there).  mid is the x87 value of the first iteration (unrounded), x the later ones (stored floats). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float rwp_math_0056f980(float x0, float x1, float f0, float f1, int n, float *poly, float *dpoly)
{
  double mid, p, d, t;
  volatile float x, delta;
  int it, j, dpos, a, b;

  mid = (x0 + x1) * 0.5f;
  if (x1 < x0) return mid;
  if (f0 * f1 >= 0.0f) return mid;
  for (it = 23; it != 0; it--) {
    p = poly[n];
    for (j = n; j != 0; ) {
      p = p * mid;
      j--;
      p = p + poly[j];
    }
    if (f0 * p < 0.0f) {
      x1 = mid;
      f1 = p;
    } else if (f1 * p < 0.0f) {
      x0 = mid;
      f0 = p;
    } else {
      return mid;
    }
    d = dpoly[n - 1];
    for (j = n - 1; j != 0; ) {
      d = d * mid;
      j--;
      d = d + dpoly[j];
    }
    dpos = d > 0.0f;
    t = (mid - x0) * d;
    t = t - p;
    a = t > 0.0f;
    if (a == dpos) {
      t = (x1 - mid) * d;
      t = t + p;
      b = t > 0.0f;
      if (b == dpos) {
        x = mid - p / d;
      } else {
        x = (x0 + x1) * 0.5f;
      }
    } else {
      x = (x0 + x1) * 0.5f;
    }
    delta = x - mid;
    if (x * x * 1.44e-14f > delta * delta) return x;
    mid = x;
  }
  return x;
}
