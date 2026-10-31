#include "mfl_tier2.h"

/*0056f980*/ extern float rwp_math_0056f980(float x0, float x1, float f0, float f1, int degree, float *poly, float *dpoly);

/* Real roots of the polynomial c[0] + c[1] x + ... + c[deg] x^deg inside [lo, hi], written ascending to roots[]; returns their number.
   c is also the work area: the derivatives are appended behind it, level L (L = 1 .. deg-1) at c + L * (deg + 1).  The roots of the derivative of
   degree n - 1 bracket the roots of the polynomial of degree n (sign change between neighbours, exact zeros kept), each bracket is refined by
   rwp_math_0056f980; the linear level is solved directly. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_math_0056fb00(float *roots, float lo, float hi, int deg, float *c)
{
  float *p, *dp, *prev;
  double r, pd, ev, phi;
  float x, xprev, pf, evf, phif;
  int l, j, k, n, cnt, found;

  if (deg == 0) return 0;
  while (c[deg] == 0.0f) {
    if (--deg == 0) return 0;
  }
  p = c;
  for (l = deg - 1; l >= 1; l--) {
    prev = p;
    p = p + (deg + 1);
    for (j = 0; j <= l; j++) p[j] = (j + 1) * prev[j + 1];
  }
  r = -(p[0] / p[1]);
  cnt = 0;
  if (r >= lo && r <= hi) {
    roots[0] = r;
    cnt = 1;
  }
  for (n = 2; n <= deg; n++) {
    dp = p;
    p = p - (deg + 1);
    pd = p[n];
    for (j = n; j != 0; ) {
      pd = pd * lo;
      j--;
      pd = pd + p[j];
    }
    pf = pd;
    xprev = lo;
    found = 0;
    for (k = 0; k < cnt; k++) {
      x = roots[k];
      ev = p[n];
      for (j = n; j != 0; ) {
        ev = ev * x;
        j--;
        ev = ev + p[j];
      }
      evf = ev;
      if (ev == 0.0f) {
        roots[found++] = x;
      } else if (pf * ev < 0.0f) {
        roots[found++] = rwp_math_0056f980(xprev, x, pf, evf, n, p, dp);
        ev = evf;
      }
      pf = ev;
      pd = ev;
      xprev = x;
    }
    phi = p[n];
    for (j = n; j != 0; ) {
      phi = phi * hi;
      j--;
      phi = phi + p[j];
    }
    phif = phi;
    if (phi * pd < 0.0f) roots[found++] = rwp_math_0056f980(xprev, hi, pf, phif, n, p, dp);
    cnt = found;
  }
  return cnt;
}
