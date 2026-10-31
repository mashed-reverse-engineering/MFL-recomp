#include "mfl_tier2.h"

/*00588280*/ extern int rwp_aggregate_00588280(float *origin, float *dir, float t_min, float t_max, float *point, float *out);

/* Closest point of the polygon outline poly[0..n-1] (16 byte vertices, xyz first) to the point p, for a point lying (nearly)
   in the plane with normal nrm: picks the edge whose side test (edge . (d x nrm), d = p - vertex) is non-negative and has the
   largest squared distance ratio (T^2 / |edge|^2, compared by cross multiplication against the running best), stores its
   index in *edge and returns the segment query rwp_aggregate_00588280 on it (out = closest point).  When no edge qualifies
   (*edge == n, the point is inside), out = p projected onto the plane through poly[0] and the return value is 2. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_aggregate_00588350(float *poly, unsigned n, float *nrm, float *p, float *out, unsigned *edge)
{
  unsigned i, next, k;
  float best_score, best_len2, len2;
  float e[3];
  double dx, dy, dz, T, T2, P, Q, R, S;
  float *v, *w;

  best_score = -3.4028235e38f;
  best_len2 = 1.0f;
  *edge = n;
  for (i = 0; i < n; i++) {
    next = ((int)(1 - n + i) >> 31) & (i + 1);
    v = poly + 4 * i;
    w = poly + 4 * next;
    e[0] = w[0] - v[0];
    e[1] = w[1] - v[1];
    e[2] = w[2] - v[2];
    dx = p[0] - v[0];
    dy = p[1] - v[1];
    dz = p[2] - v[2];
    P = dx * nrm[2] - dz * nrm[0];
    P = P * e[1];
    Q = dz * nrm[1] - dy * nrm[2];
    Q = Q * e[0];
    S = Q + P;
    R = dy * nrm[0] - dx * nrm[1];
    R = R * e[2];
    T = S + R;
    if (T < 0.0f)
      continue;
    T2 = T * T;
    len2 = e[1] * e[1] + e[0] * e[0] + e[2] * e[2];
    if (best_len2 * T2 > len2 * best_score) {
      best_score = T2;
      best_len2 = len2;
      *edge = i;
    }
  }
  if (*edge == n) {
    dx = poly[0] - p[0];
    dy = poly[1] - p[1];
    dz = poly[2] - p[2];
    S = nrm[1] * dy + dx * nrm[0];
    S = S + nrm[2] * dz;
    out[0] = S * nrm[0] + p[0];
    out[1] = S * nrm[1] + p[1];
    out[2] = S * nrm[2] + p[2];
    return 2;
  }
  k = *edge;
  next = ((int)(k + 1 - n) >> 31) & (k + 1);
  v = poly + 4 * k;
  w = poly + 4 * next;
  e[0] = w[0] - v[0];
  e[1] = w[1] - v[1];
  e[2] = w[2] - v[2];
  return rwp_aggregate_00588280(v, e, 0.0f, 1.0f, p, out);
}
