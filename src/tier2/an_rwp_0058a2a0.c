#include "mfl_tier2.h"

/* The simplex of the distance search (see rwp_aggchildren_0058a8f0): vertices of 6 floats (difference, support B) and
   the barycentric weights lam[mask][vertex] of every vertex subset. */
typedef struct MflSimplex {
  float vert[4][6];
  float pad60[16];
  float lam[16][4];   /* 0xa0 */
} MflSimplex;

/* Closest point of the vertex subset 'mask' of the simplex: out = sum(lam[mask][i] * vertex i) / sum(lam) (the support B part of the
   vertices when flag is set).  Returns 1 when the smallest weight is above -1e-5 * the largest weight (all weights usable). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_aggchildren_0058a2a0(float *out, int mask, int flag, MflSimplex *sx)
{
  float sum = 0.0f, maxw = -1.0f, w, inv;
  double minw = 0.0;
  float *v;
  int bit, i = 0;

  out[0] = 0.0f;
  out[1] = 0.0f;
  out[2] = 0.0f;
  v = sx->vert[0];
  for (bit = 1; bit <= mask; bit += bit) {
    if (mask & bit) {
      w = sx->lam[mask][i];
      if (w < minw) minw = w;
      if (w > maxw) maxw = w;
      sum = w + sum;
      out[0] = w * (flag ? v + 3 : v)[0] + out[0];
      out[1] = (flag ? v + 3 : v)[1] * w + out[1];
      out[2] = (flag ? v + 3 : v)[2] * w + out[2];
    }
    i++;
    v += 6;
  }
  if (mask >= 1 && sum != 0.0f) {
    inv = 1.0f / sum;
    out[0] = inv * out[0];
    out[1] = out[1] * inv;
    out[2] = out[2] * inv;
  }
  return minw > maxw * -0.00001f;
}
