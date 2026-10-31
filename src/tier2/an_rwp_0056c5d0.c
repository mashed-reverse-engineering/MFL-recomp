#include "mfl_tier2.h"

/* vtable slot of a shape: support function */
typedef struct RwpShapeVtbl { unsigned char pad00[0x14]; float *(*support)(void *shape, float *dir, int arg); } RwpShapeVtbl;
typedef struct RwpSupportShape { unsigned char pad00[0x5c]; RwpShapeVtbl *vtbl; } RwpSupportShape;

/* Support point of `shape` in world direction `dir`: the direction is rotated into the shape's frame by the
   transpose of the rotation part of `xf` (when given), the shape's support function is called, and the point is
   moved back by `xf`.  Returns the point (the support function's own buffer, modified in place). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float *rwp_sphere_0056c5d0(RwpSupportShape *shape, RwMatrix *xf, RwV3d *dir, int arg)
{
  double acc;
  float local[6];
  float *d;
  float *p;
  double f1, f2;
  volatile float f3;

  if (xf) {
    acc = xf->right.y * dir->y;
    acc = acc + xf->right.x * dir->x;
    acc = acc + xf->right.z * dir->z;
    local[0] = acc;
    acc = xf->up.y * dir->y;
    acc = acc + xf->up.x * dir->x;
    acc = acc + xf->up.z * dir->z;
    local[1] = acc;
    acc = xf->at.y * dir->y;
    acc = acc + xf->at.x * dir->x;
    acc = acc + xf->at.z * dir->z;
    local[2] = acc;
    d = local;
  } else {
    d = (float *)dir;
  }
  p = shape->vtbl->support(shape, d, arg);
  if (p) {
    if (xf) {
      acc = xf->at.x * p[2];
      acc = acc + xf->up.x * p[1];
      acc = acc + xf->right.x * p[0];
      f1 = acc;
      acc = xf->at.y * p[2];
      acc = acc + xf->up.y * p[1];
      acc = acc + xf->right.y * p[0];
      f2 = acc;
      acc = xf->at.z * p[2];
      acc = acc + xf->up.z * p[1];
      acc = acc + xf->right.z * p[0];
      f3 = acc;
      p[0] = xf->pos.x + f1;
      p[1] = xf->pos.y + f2;
      p[2] = xf->pos.z + f3;
    }
  }
  return p;
}
