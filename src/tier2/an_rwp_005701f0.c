/* rwp_cylinder_support_points (0x005701f0): support points of a cylinder lying along an axis */
#include "mfl_tier2.h"
#include <math.h>

extern float rwp_cylinder_0056fd20(float *out, float *in);

typedef struct MflCylV3 {
  float x, y, z;
} MflCylV3;

/* Cylinder / capsule parameters: half height at 0x40, radius at 0x44. */
typedef struct MflCylParams {
  unsigned char pad00[0x40];
  float half;   /* 0x40 */
  float radius;   /* 0x44 */
} MflCylParams;

/* The support description built for a cylinder along `axis`: n = axis, d = offset along it, kind 1 / 2 = one / two end points inline at 0x10 / 0x20,
   kind 8 = a 31 word block behind `ext` (the caller provides it) holding the eight corner points of the end faces. */
typedef struct MflCylOut {
  MflCylV3 n;   /* 0x0 */
  float d;   /* 0xc */
  MflCylV3 v0;   /* 0x10 */
  int pad1c;
  MflCylV3 v1;   /* 0x20 */
  unsigned char pad2c[0x24];
  float *ext;   /* 0x50 */
  unsigned short kind;   /* 0x54 */
} MflCylOut;

/* Builds the support points of a cylinder lying along `axis`: nearly parallel to z (|z| > 0.9998): the 8 corners of a box-like end block (kind 8,
   mirrored by `flip` when z < 0), nearly perpendicular (|z| <= 0.02): the two end points +-half along z (kind 2), otherwise one end point
   (kind 1).  Returns out. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
MflCylOut *rwp_cylinder_support_points(MflCylParams *p, int flip, float *axis, MflCylOut *o)
{
  MflCylV3 v;
  float vz, s, R, D, q;
  int qb;
  int *bi;
  float *B;

  v = *(MflCylV3 *)axis;
  vz = v.z;
  if (fabs(vz) > 0.9998f) {
    o->n.x = 0.0f;
    o->n.y = 0.0f;
    o->n.z = 1.0f;
    B = o->ext;
    bi = (int *)B;
    if (vz < 0.0f) {
      *(unsigned int *)&o->n.x = 0x80000000;      /* -0.0f: the original negates the stored (0, 0, 1) in memory, cl folds -(0.0f) to +0 */
      *(unsigned int *)&o->n.y = 0x80000000;
      o->n.z = -1.0f;
      s = -1.0f;
      flip = (flip == 0);
    } else {
      s = 1.0f;
    }
    R = s * p->half;
    D = p->radius * 0.70710677f;
    q = p->radius;
    qb = *(int *)&p->radius;      /* the stores below may overwrite p->radius (aliased output): the original works from a saved copy */
    bi[0] = 0;
    bi[1] = qb;
    B[2] = R;
    if (flip == 0) {
      B[4] = D; B[5] = D; B[6] = R;
      bi[8] = qb; bi[9] = 0; B[10] = R;
      B[12] = D; B[13] = -D; B[14] = R;
      bi[16] = 0; B[17] = -q; B[18] = R;
      B[20] = -D; B[21] = -D; B[22] = R;
      B[24] = -q; bi[25] = 0; B[26] = R;
      B[28] = -D;
    } else {
      B[4] = -D; B[5] = D; B[6] = R;
      B[8] = -q; bi[9] = 0; B[10] = R;
      B[12] = -D; B[13] = -D; B[14] = R;
      bi[16] = 0; B[17] = -q; B[18] = R;
      B[20] = D; B[21] = -D; B[22] = R;
      bi[24] = qb; bi[25] = 0; B[26] = R;
      B[28] = D;
    }
    B[29] = D;
    B[30] = R;
    o->d = R;
    o->kind = 8;
    return o;
  }
  v.z = 0.0f;
  if (fabs(vz) > 0.02f) {
    rwp_cylinder_0056fd20(&v.x, &v.x);
    v.x = p->radius * v.x;
    v.y = p->radius * v.y;
    v.z = (vz < 0.0f ? -1.0f : 1.0f) * p->half;
    o->ext = &o->v0.x;
    o->v0 = v;
    o->kind = 1;
    o->n = *(MflCylV3 *)axis;
    o->d = o->v0.y * o->n.y + o->v0.x * o->n.x + o->v0.z * o->n.z;
    return o;
  }
  rwp_cylinder_0056fd20(&v.x, &v.x);
  v.x = p->radius * v.x;
  v.y = p->radius * v.y;
  *(int *)&v.z = *(int *)&p->half;
  o->ext = &o->v0.x;
  o->v0 = v;
  v.z = -p->half;
  o->v1 = v;
  o->kind = 2;
  o->n = *(MflCylV3 *)axis;
  o->d = o->n.y * v.y + v.x * o->n.x + o->n.z * v.z;
  return o;
}
