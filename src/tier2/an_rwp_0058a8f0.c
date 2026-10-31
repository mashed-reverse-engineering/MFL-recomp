#include "mfl_tier2.h"
#include <math.h>

extern float rwp_cylinder_0056fd20(float *out, float *in);
extern float rwp_aggchildren_0058a3f0(float *out, float *dir, int mode, void *pair, void *flag);
extern int rwp_aggchildren_0058a2a0(float *out, int mask, int flag, void *simplex);
extern void rwp_aggchildren_0058a4f0(void *simplex);
extern void rwp_aggchildren_0058a580(void *simplex);
extern int *g_rwp_simplex_face_lists[16];

/* The simplex of the distance search: up to four vertices (24 bytes each, the point first), the mask of the used vertices and the
   slot / bit chosen for the next one. */
typedef struct MflSimplex {
  float vert[4][6];
  unsigned char pad60[0x144];
  int mask;   /* 0x1a4 */
  int slot;   /* 0x1a8 */
  int bit;    /* 0x1ac */
} MflSimplex;

/* Iterative closest point search of a simplex against the pair (GJK style): each round adds a support vertex in the direction best[]
   and keeps the face of the simplex nearest the origin.  Stops when the direction length len falls below step * 1e-4 (result 1, *out_depth = -1),
   when the support distance exceeds t (result 2) or the estimate is good enough (result 1); more than 50 rounds or more than 5 repeated vertices give 0.
   On a result 1 / 2 exit point = the closest point direction, *out_a = point . best + dist * 0.5 and *out_depth = dist (the last support distance). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_aggchildren_0058a8f0(void *pair, void *flag, float t, MflSimplex *sx, float *point, float *out_a, float *out_depth, float step)
{
  float tol, lim, gmax, dist, bestlen, len2;
  float best[3] = { 0.5f, 0.6f, 0.6244998f }, hist[2][3], c[3];
  double len, acc;
  float *v;
  int iter, repeats, bit, slot, mask, k, face, bestface, res;

  tol = step * 0.0001f;
  len = tol * 1.1f;
  len = len + t;
  len = len + 1.0f;
  lim = len;
  gmax = 0.0f;
  bestlen = 0.0f;
  repeats = 0;
  iter = 1;
  sx->mask = 0;
  while (len > tol) {
    mask = sx->mask;
    bit = 1;
    slot = 0;
    sx->bit = 1;
    sx->slot = 0;
    if (mask & 1) {
      do {
        bit += bit;
        slot++;
      } while (mask & bit);
      sx->slot = slot;
      sx->bit = bit;
    }
    v = sx->vert[sx->slot];
    dist = rwp_aggchildren_0058a3f0(v, best, -1, pair, flag);
    if (dist > t) { res = 2; goto done; }
    if (dist > gmax) gmax = dist;
    if (gmax * 1.01f > lim) { res = 1; goto done; }
    if (lim > 0.0f && dist > 0.0f) {
      if (repeats != 0 || iter > 40) { res = 1; goto done; }
    }
    iter++;
    if (iter > 50) return 0;
    if (iter > 3) {
      acc = fabs(v[0] - hist[0][0]);
      acc = acc + fabs(v[1] - hist[0][1]);
      acc = acc + fabs(v[2] - hist[0][2]);
      if (acc < 1e-6f) {
        repeats++;
        if (repeats > 5) return 0;
      } else {
        acc = fabs(v[0] - hist[1][0]);
        acc = acc + fabs(v[1] - hist[1][1]);
        acc = acc + fabs(v[2] - hist[1][2]);
        if (acc < 1e-6f) {
          repeats++;
          if (repeats > 5) return 0;
        }
      }
    }
    *(int *)&hist[iter & 1][0] = *(int *)&v[0];
    *(int *)&hist[iter & 1][1] = *(int *)&v[1];
    *(int *)&hist[iter & 1][2] = *(int *)&v[2];
    rwp_aggchildren_0058a4f0(sx);
    rwp_aggchildren_0058a580(sx);
    bestface = 0;
    k = 0;
    do {
      face = g_rwp_simplex_face_lists[sx->mask][k];
      if (k == 0 && gmax > 0.0f && (sx->bit | face) == 15) {
        k++;
        continue;
      }
      if (face == 0) {
        v = sx->vert[sx->slot];
        *(int *)&c[0] = *(int *)&v[0];
        *(int *)&c[1] = *(int *)&v[1];
        *(int *)&c[2] = *(int *)&v[2];
      } else if (!rwp_aggchildren_0058a2a0(c, sx->bit | face, 0, sx)) {
        k++;
        continue;
      }
      len2 = c[1] * c[1];
      len2 = len2 + c[0] * c[0];
      len2 = len2 + c[2] * c[2];
      if (bestface == 0 || len2 <= bestlen) {
        bestface = face;
        bestlen = len2;
        *(int *)&best[0] = *(int *)&c[0];
        *(int *)&best[1] = *(int *)&c[1];
        *(int *)&best[2] = *(int *)&c[2];
      }
      k++;
    } while (face != 0);
    len = rwp_cylinder_0056fd20(best, best);
    lim = len;
    sx->mask = sx->bit | bestface;
    if (sx->mask >= 15) break;
  }
  *out_depth = -1.0f;
  return 1;
done:
  *(int *)&point[0] = *(int *)&best[0];
  *(int *)&point[1] = *(int *)&best[1];
  *(int *)&point[2] = *(int *)&best[2];
  rwp_aggchildren_0058a2a0(best, sx->mask, 1, sx);
  acc = point[1] * best[1];
  acc = acc + best[0] * point[0];
  acc = acc + point[2] * best[2];
  acc = acc + dist * 0.5f;
  *out_a = acc;
  *out_depth = dist;
  return res;
}
