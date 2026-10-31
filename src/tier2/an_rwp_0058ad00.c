#include "mfl_tier2.h"

extern float rwp_cylinder_0056fd20(float *out, float *in);
extern void *RpDMorphTargetGetBoundingSphere(void *o);
extern void rwp_sphere_0056c340(void *shape, void *xf, int unused, float *out);
extern float rwp_aggchildren_0058a3f0(float *out, float *dir, int mode, void *pair, int flag);

typedef struct MflMat { unsigned short kind; } MflMat;
typedef struct MflShp {
  unsigned char pad00[0x48];
  unsigned char flags;   /* 0x48 */
  unsigned char pad49[0x13];
  MflMat *material;   /* 0x5c */
} MflShp;
typedef struct MflBody {
  int id0, id1;
  MflShp *shape;   /* 0x8 */
  int id3;
  void *xf;   /* 0x10 */
} MflBody;

/* Separating direction of a pair of bodies (pair[0], pair[1]), written to dir: a material kind 4 shape with flag 1 gives its fixed vector
   (negated for the first body); otherwise the difference of the two shapes' mid points, normalised by rwp_cylinder_0056fd20.
   The direction and the three axes (sign of the difference component) are then scored with rwp_aggchildren_0058a3f0 and the one with the
   smallest negated score is kept in dir.  Returns that score. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float rwp_gjk_0058ad00(MflBody **pair, float *dir)
{
  MflShp *s1 = pair[0]->shape, *s2;
  float *v;
  float sph[8], m[6] /* m[0..2] = mid point of body 0, m[3..5] of body 1, then their difference in m[0..2] */, cand[3], scratch[6];
  float best, nv;

  if (s1->material->kind == 4 && (s1->flags & 1)) {
    v = (float *)RpDMorphTargetGetBoundingSphere(s1);
    dir[0] = v[0] * -1.0f;
    dir[1] = v[1] * -1.0f;
    dir[2] = v[2] * -1.0f;
  } else {
    s2 = pair[1]->shape;
    if (s2->material->kind == 4 && (s2->flags & 1)) {
      v = (float *)RpDMorphTargetGetBoundingSphere(s2);
      *(int *)&dir[0] = *(int *)&v[0];
      *(int *)&dir[1] = *(int *)&v[1];
      *(int *)&dir[2] = *(int *)&v[2];
    } else {
      rwp_sphere_0056c340(s1, pair[0]->xf, 0, sph);
      m[0] = (sph[0] - sph[4]) * 0.5f + sph[4];
      m[1] = (sph[1] - sph[5]) * 0.5f + sph[5];
      m[2] = (sph[2] - sph[6]) * 0.5f + sph[6];
      rwp_sphere_0056c340(pair[1]->shape, pair[1]->xf, 0, sph);
      m[3] = (sph[0] - sph[4]) * 0.5f + sph[4];
      m[4] = (sph[1] - sph[5]) * 0.5f + sph[5];
      m[5] = (sph[2] - sph[6]) * 0.5f + sph[6];
      m[0] = m[0] - m[3];
      m[1] = m[1] - m[4];
      m[2] = m[2] - m[5];
      rwp_cylinder_0056fd20(dir, m);
    }
  }
  best = -rwp_aggchildren_0058a3f0(scratch, dir, -1, pair, 0);

  cand[0] = !(m[0] < 0.0f) ? 1.0f : -1.0f;
  cand[1] = 0.0f;
  cand[2] = 0.0f;
  nv = -rwp_aggchildren_0058a3f0(scratch, cand, -1, pair, 0);
  if (best > nv) {
    best = nv;
    dir[0] = cand[0]; dir[1] = cand[1]; dir[2] = cand[2];
  }
  cand[0] = 0.0f;
  cand[1] = !(m[1] < 0.0f) ? 1.0f : -1.0f;
  cand[2] = 0.0f;
  nv = -rwp_aggchildren_0058a3f0(scratch, cand, -1, pair, 0);
  if (best > nv) {
    best = nv;
    dir[0] = cand[0]; dir[1] = cand[1]; dir[2] = cand[2];
  }
  cand[0] = 0.0f;
  cand[1] = 0.0f;
  cand[2] = !(m[2] < 0.0f) ? 1.0f : -1.0f;
  nv = -rwp_aggchildren_0058a3f0(scratch, cand, -1, pair, 0);
  if (best > nv) {
    dir[0] = cand[0]; dir[1] = cand[1]; dir[2] = cand[2];
    return nv;
  }
  return best;
}
