#include "mfl_tier2.h"

/*0056c5d0*/ extern float *rwp_sphere_0056c5d0(void *shape, float *xf, float *dir, float *out);
/*0058a8f0*/ extern int rwp_aggchildren_0058a8f0(void *pair, RwV3d *dir, float t, float *contacts, RwV3d *point, float *out_a, float *out_depth, float step);
/*0058ad00*/ extern float rwp_gjk_0058ad00(void *pair, float *dir);

typedef struct RwpMat { unsigned short kind; } RwpMat;
typedef struct RwpShp {
  unsigned char pad00[0x4c];
  float radius;   /* 0x4c */
  unsigned char pad50[0xc];
  RwpMat *material;   /* 0x5c */
} RwpShp;
typedef struct RwpBody {
  int id0, id1;
  RwpShp *shape;   /* 0x8 */
  int id3;
  float *xf;   /* 0x10 */
} RwpBody;

/* Separation query for a pair of bodies (pair[0], pair[1]): a sphere-like shape (material kind 1) is answered directly from the support point of
   the other shape; otherwise the direction n from rwp_gjk_0058ad00 is searched for the contact distance with at most four rounds of
   bracketing (t between lo and hi) around rwp_aggchildren_0058a8f0.  *pa receives the mid point and *pb the distance (both relative to the radii).
   Returns 0 when no contact was found, 1 otherwise. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_gjk_0058b040(int unused, RwpBody **pair, float margin, float *dir, float *pa, float *pb, float step)
{
  double acc0;
  RwpBody *b1 = pair[0], *b2 = pair[1];
  RwpShp *s1 = b1->shape, *s2 = b2->shape;
  float r1 = s1->radius, r2 = s2->radius;
  float sup[3], n[3], pt[3], rest[100];
  RwV3d *supv = (RwV3d *)sup, *ptv = (RwV3d *)pt;
  float t, lo, hi, d_a, depth, dotn, rfar, rsum, limit;
  int found, i;
  double A, B, f4, f8;
  float dd;

  if (s1->material->kind == 1) {
    dir[0] = -b1->xf[8];
    dir[1] = -b1->xf[9];
    dir[2] = -b1->xf[10];
    rwp_sphere_0056c5d0(b2->shape, b2->xf, dir, sup);
    acc0 = dir[1] * sup[1];
    acc0 = acc0 + sup[0] * dir[0];
    acc0 = acc0 + dir[2] * sup[2];
    acc0 = acc0 + r2;
    A = acc0;
    B = b1->xf[13] * dir[1] + b1->xf[12] * dir[0] + b1->xf[14] * dir[2] - r1;
  } else {
    if (!(s2->material->kind == 1)) goto L_58b1a6;
    dir[0] = b2->xf[8];
    dir[1] = b2->xf[9];
    dir[2] = b2->xf[10];
    rwp_sphere_0056c5d0(b1->shape, b1->xf, dir, sup);
    acc0 = dir[1] * sup[1];
    acc0 = acc0 + sup[0] * dir[0];
    acc0 = acc0 + dir[2] * sup[2];
    acc0 = acc0 + r1;
    A = acc0;
    B = b2->xf[13] * dir[1] + b2->xf[12] * dir[0] + b2->xf[14] * dir[2] - r2;
  }
  *pa = (B + A) * 0.5f;
  *pb = B - A;
  return 1;
L_58b1a6:;
  d_a = rwp_gjk_0058ad00(pair, n);
  f4 = step * 0.01f;
  if (d_a > f4) f4 = f4 + d_a;
  hi = f4;
  rsum = r2 + r1;
  lo = 0.0f;
  t = 0.0f;
  rfar = rsum;
  limit = (r2 + r1) + margin;
  found = 0;
  for (i = 0; i < 4; i++) {
    sup[0] = n[0] * t;
    sup[1] = n[1] * t;
    sup[2] = n[2] * t;
    if (rwp_aggchildren_0058a8f0(pair, supv, limit + t, rest, ptv, &d_a, &depth, step) && depth > 0.0f) {
      hi = t;
      found = 1;
      dir[0] = pt[0];
      dir[1] = pt[1];
      dir[2] = pt[2];
      *pa = d_a;
      *pb = depth;
      if (t == 0.0f) goto L_58b41f;
      if (depth < margin) goto L_58b3dc;
      acc0 = dir[1] * n[1];
      acc0 = acc0 + n[0] * dir[0];
      acc0 = acc0 + dir[2] * n[2];
      d_a = acc0;
      f8 = depth / d_a;
      if (t > f8) t = t - f8 * 0.95f; else t = t * 0.05f + lo * 0.95f;
      if (lo > t) lo = t;
    } else {
      lo = t;
      if (found) {
        t = (t + hi) * 0.5f;
      } else {
        if (t == hi) hi = hi + step;
        t = hi;
      }
    }
  }
  if (!found) return 0;
L_58b3dc:;
  if (hi != 0.0f) {
    acc0 = dir[1] * n[1];
    acc0 = acc0 + n[0] * dir[0];
    acc0 = acc0 + dir[2] * n[2];
    dd = acc0 * hi;
    *pa = *pa - 0.5f * dd;
    *pb = *pb - dd;
  }
L_58b41f:;
  *pa = *pa - (r1 - r2) * 0.5f;
  *pb = *pb - rsum;
  return 1;
}
