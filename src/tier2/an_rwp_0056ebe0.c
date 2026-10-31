#include "mfl_tier2.h"
#include <math.h>

extern void rwp_obj_0056ee50(float *m, int i, int j, float c, float s);

/* Cyclic Jacobi sweeps on a symmetric 3x3 matrix a (floats, stride 12 bytes per row; only the lower triangle is read): while the sum of the
   squared lower off-diagonal elements is above tol^2 and sweeps are left, every pair (p, q) gets the rotation that clears a[p][q]
   (angle from the diagonal difference), applied to the eigenvector matrix v (set to the identity first) and to a. Returns the number of
   sweeps left. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_obj_0056ebe0(float *a, float *v, int maxit, float tol)
{
  float tol2 = tol * tol;
  volatile float cs[2], crd;      /* cosine, sine: frame floats (the original stores the cosine and forms the sine from the stored, rounded value) */
  double theta, t;
  float *rowp, *rowq, *apq_p, *diag_p, *diag_q;
  float x2, y0, y1, y2;
  int p, q;
  double norm;

  v[8] = 1.0f; v[4] = 1.0f; v[0] = 1.0f;
  v[7] = 0.0f; v[6] = 0.0f; v[3] = 0.0f; v[5] = 0.0f; v[2] = 0.0f; v[1] = 0.0f;
  norm = a[3] * a[3];
  norm = norm + a[6] * a[6];
  norm = norm + a[7] * a[7];
  if (!(norm > tol2) || maxit == 0) return maxit;
  for (;;) {
    rowp = a + 3;
    diag_p = a + 4;
    for (p = 1; p < 3; p++) {
      apq_p = rowp;
      rowq = a + 2;
      diag_q = a;
      for (q = 0; q < p; q++) {
        cs[0] = 1.0f;
        cs[1] = 0.0f;
        if (*apq_p != 0.0f) {
          theta = (*diag_q - *diag_p) * (0.5f / *apq_p);
          if (theta > 0.0f) t = 1.0f / (theta + sqrt(theta * theta + 1.0f));
          else t = -1.0f / (sqrt(theta * theta + 1.0f) - theta);
          cs[0] = 1.0f / sqrt(t * t + 1.0f);
          crd = cs[0];
          cs[1] = t * crd;
        }
        rwp_obj_0056ee50(v, p, q, cs[0], cs[1]);
        rwp_obj_0056ee50(a, p, q, cs[0], cs[1]);
        rowq += 3;
        x2 = cs[0] * rowp[2] - cs[1] * rowq[-3];
        y0 = cs[1] * rowp[0] + cs[0] * rowq[-5];
        y1 = cs[0] * rowq[-4] + cs[1] * rowp[1];
        y2 = cs[1] * rowp[2] + cs[0] * rowq[-3];
        t = cs[0] * rowp[0] - cs[1] * rowq[-5];
        theta = cs[0] * rowp[1] - cs[1] * rowq[-4];
        rowp[2] = x2;
        rowp[0] = t;
        rowp[1] = theta;
        rowq[-5] = y0;
        rowq[-4] = y1;
        rowq[-3] = y2;
        apq_p += 1;
        diag_q += 4;
      }
      rowp += 3;
      diag_p += 4;
    }
    maxit--;
    norm = a[3] * a[3];
    norm = norm + a[6] * a[6];
    norm = norm + a[7] * a[7];
    if (!(norm > tol2)) break;
    if (maxit == 0) break;
  }
  return maxit;
}
