#include "mfl_tier2.h"

/* A shape object: orientation rows, position, flags, two scale factors and the vtable at 0x5c. */
typedef struct MflShapeObj {
  float right[3];   /* 0x00 */
  unsigned int flags;   /* 0x0c */
  float up[3];   /* 0x10 */
  int pad1c;
  float at[3];   /* 0x20 */
  int pad2c;
  float pos[3];   /* 0x30 */
  int pad3c[4];
  float f4c;   /* 0x4c */
  float scale_a, scale_b;   /* 0x50, 0x54 */
  short s58, s5a;
  void *ops;   /* 0x5c */
} MflShapeObj;

/* Resets a shape object to the identity frame at the origin (flags |= 0x20003, scales 0.2) and sets its vtable. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_sphere_0056c950(MflShapeObj *o, void *ops)
{
  o->f4c = 0.0f;
  o->s5a = 0;
  o->s58 = 0;
  o->up[0] = 0.0f;
  o->right[2] = 0.0f;
  o->right[1] = 0.0f;
  o->at[1] = 0.0f;
  o->at[0] = 0.0f;
  o->up[2] = 0.0f;
  o->pos[2] = 0.0f;
  o->pos[1] = 0.0f;
  o->pos[0] = 0.0f;
  o->scale_b = 0.2f;
  o->scale_a = 0.2f;
  o->flags = o->flags | 0x20003;
  o->ops = ops;
  o->at[2] = 1.0f;
  o->up[1] = 1.0f;
  o->right[0] = 1.0f;
}
