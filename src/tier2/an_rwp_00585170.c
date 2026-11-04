#include "mfl_tier2.h"
#include <math.h>
#include <float.h>
extern int rwp_aggregate_005857c0(float *pos, float tag, void *list, float mindist);
extern float rwp_aggregate_00585890(float *a, float *b, float *c);

/* rwp_aggregate_00585170: contact generation for a face against a slab.  P holds two planes (normal a at +0x0 / offset +0xc, normal b at +0x80 / offset +0x8c)
   and the direction c at +0x100; V = n vertices (16 bytes each) of a polygon.  The signed distance of a vertex is w = (tb - ta) - f4 - f5 with tb / ta the
   plane distances along c; the polygon is clipped to w <= 0 (Sutherland-Hodgman, the vertex of smallest w is kept), reduced to 4 points by repeatedly
   dropping the vertex whose triangle (585890) is smallest, and every remaining point is added to the contact list with tag f4 + w (5857c0).
   n == 1: that single point is added with tag (tb - ta) - f5 and the result of that add is returned.  Returns 1 otherwise. */
typedef struct MflSlab {
  float a[3];
  float a_off;       /* 0xc */
  char pad10[0x70];
  float b[3];        /* 0x80 */
  float b_off;       /* 0x8c */
  char pad90[0x70];
  float c[3];        /* 0x100 */
} MflSlab;

/* TIER2: unvalidated 2026-11-02 authority=diffemu */
int rwp_aggregate_00585170(MflSlab *P, float *V, unsigned int n, float f4, float f5, void *list)
{
  /* the three arrays lie back to back in the original frame (esp+0x38 / +0x78 / +0xd8, out up to the saved registers at +0x1f8); nothing
     bounds n or the clipped count, so a polygon of more than 16 vertices runs w into w2 and out exactly as there: one block keeps that layout */
  float blk[16 + 24 + 72];
  float *w = blk;           /* [16] distances, later the triangle costs (-1 = dropped) */
  float *w2 = blk + 16;     /* [24] distance of every clipped point */
  float *out = blk + 40;    /* [72] clipped polygon, 3 floats each */
  float d1, d2, tb, ta, s, num, eps, mn, t, tag;
  double acc;
  volatile double db, da;      /* the original keeps the two divisors in x87 registers and divides per vertex; cl turns a division by a loop invariant into a reciprocal multiply */
  float *v, *q, *r;
  unsigned int k, j, jp, m, best, best_out, a, b, c, cnt, nx1, nx2, pv1, pv2, rounds;
  int *iw = (int *)w;

  if (n == 0) return 1;
  if (n == 1) {
    s = P->b[1] * V[1];
    s = s + P->b[0] * V[0];
    s = s + P->b[2] * V[2];
    s = s - P->b_off;
    d1 = P->b[1] * P->c[1];
    d1 = d1 + P->c[0] * P->b[0];
    d1 = d1 + P->c[2] * P->b[2];
    tb = s / d1;
    s = V[1] * P->a[1];
    s = s + P->a[0] * V[0];
    s = s + P->a[2] * V[2];
    s = s - P->a_off;
    d2 = P->c[1] * P->a[1];
    d2 = d2 + P->c[0] * P->a[0];
    d2 = d2 + P->a[2] * P->c[2];
    ta = s / d2;
    tag = tb - ta;
    tag = tag - f5;
    return rwp_aggregate_005857c0(V, tag, list, 0.0f);      /* the original returns the add's result here (eax not reloaded) */
  }
  acc = 0.0;
  best = 0;
  db = P->b[1] * P->c[1];
  db = db + P->c[0] * P->b[0];
  db = db + P->c[2] * P->b[2];
  da = P->a[1] * P->c[1];
  da = da + P->c[0] * P->a[0];
  da = da + P->c[2] * P->a[2];
  for (k = 0, cnt = n; cnt != 0; cnt--, k++) {
    v = V + 4 * k;
    s = v[0] * P->b[0];
    s = s + v[1] * P->b[1];
    s = s + v[2] * P->b[2];
    s = s - P->b_off;
    tb = s / db;
    s = v[1] * P->a[1];
    s = s + v[0] * P->a[0];
    s = s + P->a[2] * v[2];
    s = s - P->a_off;
    ta = s / da;
    s = tb - ta;
    s = s - f4;
    s = s - f5;
    w[k] = s;
    if (s < w[best]) best = k;
    if (k != 0) {
      acc = acc + (fabs(V[2] - v[2]) + fabs(V[1] - v[1]) + fabs(V[0] - v[0]));
    }
  }
  if (w[best] > 0.0f) return 1;
  eps = (float)((0.01f / (double)(n - 1)) * acc);
  best_out = 16;
  m = 0;
  jp = n - 1;      /* previous vertex */
  q = V + 4 * (n - 1);
  /* w[j] and w[jp] are read again after every store to w2: with n > 16 they alias w2[m] (see blk), and the original reads them from memory */
  for (j = 0, cnt = n; cnt != 0; cnt--, j++) {
    v = V + 4 * j;
    if (w[j] <= 0.0f) {
      if (w[jp] > 0.0f) {
        w2[m] = 0.0f;
        t = w[jp] / (w[jp] - w[j]);
        out[3 * m] = (v[0] - q[0]) * t + q[0];
        out[3 * m + 1] = (v[1] - q[1]) * t + q[1];
        out[3 * m + 2] = (v[2] - q[2]) * t + q[2];
        m++;
      }
      out[3 * m] = v[0]; out[3 * m + 1] = v[1]; out[3 * m + 2] = v[2];
      w2[m] = w[j];
      if (best == j) best_out = m;
      m++;
    } else {
      if (w[jp] <= 0.0f) {
        w2[m] = 0.0f;
        t = w[jp] / (w[jp] - w[j]);
        out[3 * m] = (v[0] - q[0]) * t + q[0];
        out[3 * m + 1] = (v[1] - q[1]) * t + q[1];
        out[3 * m + 2] = (v[2] - q[2]) * t + q[2];
        m++;
      }
    }
    jp = j;
    q = v;
  }
  if (m > 4) {
    a = m - 2; b = m - 1; c = 0;
    for (cnt = 0; cnt < m; cnt++) {
      if (b == best_out) w[b] = FLT_MAX;
      else w[b] = rwp_aggregate_00585890(&out[3 * a], &out[3 * b], &out[3 * c]);
      a = b; b = cnt; c = cnt + 1;
    }
    for (rounds = m - 4; rounds != 0; rounds--) {
      mn = FLT_MAX;
      a = m;
      for (k = 0; k < m; k++) {
        if (w[k] != -1.0f) {
          if (w[k] < mn) { mn = w[k]; a = k; }
        }
      }
      if (a != m) {
        w[a] = -1.0f;
        nx1 = a;
        for (cnt = 0; cnt < m; cnt++) {
          nx1 = nx1 + 1;
          if (nx1 >= m) nx1 = 0;
          if (iw[nx1] != 0xbf800000) break;
        }
        nx2 = nx1;
        for (cnt = 0; cnt < m; cnt++) {
          nx2 = nx2 + 1;
          if (nx2 >= m) nx2 = 0;
          if (iw[nx2] != 0xbf800000) break;
        }
        pv1 = a;
        for (cnt = 0; cnt < m; cnt++) {
          pv1 = pv1 == 0 ? m - 1 : pv1 - 1;
          if (iw[pv1] != 0xbf800000) break;
        }
        pv2 = pv1;
        for (cnt = 0; cnt < m; cnt++) {
          pv2 = pv2 == 0 ? m - 1 : pv2 - 1;
          if (iw[pv2] != 0xbf800000) break;
        }
        w[pv1] = rwp_aggregate_00585890(&out[3 * pv2], &out[3 * pv1], &out[3 * nx1]);
        w[nx1] = rwp_aggregate_00585890(&out[3 * pv1], &out[3 * nx1], &out[3 * nx2]);
      }
    }
    for (k = 0; k < m; k++)
      if (iw[k] != 0xbf800000) rwp_aggregate_005857c0(&out[3 * k], f4 + w2[k], list, eps);
    return 1;
  }
  for (k = 0; k < m; k++) rwp_aggregate_005857c0(&out[3 * k], f4 + w2[k], list, eps);
  return 1;
}
