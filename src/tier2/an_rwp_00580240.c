#include "mfl_tier2.h"

/* Four words that are moved as integers and used as floats. */
typedef union MflQuat {
  float f[4];
  int w[4];
} MflQuat;
typedef union MflVec3 {
  float f[3];
  int w[3];
} MflVec3;
/* The tables of a physics world: body matrices (64 bytes each) at 0 and body orientation quaternions at 0x10. */
typedef struct MflWorldTables {
  float *mats;   /* 0x0 */
  unsigned char pad04[0xc];
  MflQuat *quats;   /* 0x10 */
} MflWorldTables;
typedef struct MflWorldRef {
  unsigned char pad00[0x10];
  MflWorldTables *tables;   /* 0x10 */
} MflWorldRef;
/* The body record of a handle (see the body list entries): world, body index and the local position of the shape. */
typedef struct MflBodyEntry {
  MflWorldRef *world;   /* 0x0 */
  int index;   /* 0x4 */
  MflVec3 local;   /* 0x8 */
} MflBodyEntry;
/* One entry of an object's body list (12 bytes): body record at 4, plain matrix at 8. */
typedef struct MflListEntry {
  int pad0;
  MflBodyEntry *body;   /* 0x4 */
  float *mat;   /* 0x8 */
} MflListEntry;
typedef struct MflTypeData {
  unsigned char pad00[0x40];
  unsigned char flags;   /* 0x40 */
} MflTypeData;
typedef struct MflShapeObj {
  unsigned char pad00[0x10];
  MflListEntry *list;   /* 0x10 */
  unsigned char pad14[0x48];
  MflTypeData *data;   /* 0x5c */
} MflShapeObj;
typedef struct MflShapeHandle {
  MflShapeObj *obj;
  unsigned short id;   /* 0x4: 0xffff = the object itself */
  unsigned short pad;
} MflShapeHandle;

extern void RtQuatConvertFromMatrix(MflQuat *q, float *mat);

/* Frame of a shape relative to its body: finds the body record, matrix and orientation q of the handle (an indexed body of a world, a plain
   matrix converted to a quaternion by RtQuatConvertFromMatrix, or the object itself), multiplies q with the relative orientation qb (qb[0..3]) into qout, writes the
   rotation matrix of qout to mout (flags 3, position set from the offset qb[4..6] - local position, rotated by the body matrix, plus
   the body position) and stores the address of the body matrix' position in *pos_out.  Returns the body record, or 0. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
MflBodyEntry *rwp_constraints_00580240(MflShapeHandle *h, float *qb, float *qout, float **pos_out, float *mout)
{
  MflBodyEntry *body;
  MflWorldTables *tabs;
  MflQuat q;
  MflVec3 lp;
  float *m;
  volatile float xx, yy, zz, zy, zx, yx;      /* rounded to frame floats like the original (fstp dword [esp+N], reloaded) */
  double x_w, w_y, w_z;
  float x, y, z, w;
  double d_x, d_y, d_z, pz, dot;
  volatile float px, py;      /* frame floats, see above */

  if (h->id != 0xffff) {
    body = h->obj->list[h->id].body;
    if (body) {
      tabs = body->world->tables;
      m = tabs->mats + body->index * 16;
      lp.w[0] = body->local.w[0];
      lp.w[1] = body->local.w[1];
      lp.w[2] = body->local.w[2];
      q.w[0] = tabs->quats[body->index].w[0];
      q.w[1] = tabs->quats[body->index].w[1];
      q.w[2] = tabs->quats[body->index].w[2];
      q.w[3] = tabs->quats[body->index].w[3];
      goto have_frame;
    }
  } else {
    body = 0;
  }
  if (h->id != 0xffff) m = h->obj->list[h->id].mat;
  else m = (h->obj->data->flags & 2) ? 0 : (float *)h->obj;
  lp.f[0] = 0.0f;
  lp.f[1] = 0.0f;
  lp.f[2] = 0.0f;
  RtQuatConvertFromMatrix(&q, m);
have_frame:
  *pos_out = m + 12;
  dot = qb[1] * q.f[1];
  dot = dot + q.f[0] * qb[0];
  dot = dot + qb[2] * q.f[2];
  qout[3] = q.f[3] * qb[3] - dot;
  qout[0] = qb[2] * q.f[1] - qb[1] * q.f[2];
  qout[1] = q.f[2] * qb[0] - qb[2] * q.f[0];
  qout[2] = qb[1] * q.f[0] - q.f[1] * qb[0];
  qout[0] = q.f[3] * qb[0] + qout[0];
  qout[1] = qb[1] * q.f[3] + qout[1];
  qout[2] = qb[2] * q.f[3] + qout[2];
  qout[0] = qb[3] * q.f[0] + qout[0];
  qout[1] = qb[3] * q.f[1] + qout[1];
  qout[2] = qb[3] * q.f[2] + qout[2];
  x = qout[0];
  y = qout[1];
  z = qout[2];
  w = qout[3];
  xx = x * x;
  yy = y * y;
  zz = z * z;
  zy = z * y;
  zx = z * x;
  yx = y * x;
  x_w = x * w;
  w_y = w * y;
  w_z = w * z;
  mout[0] = 1.0f - ((zz + yy) + (zz + yy));
  mout[1] = (yx + w_z) + (yx + w_z);
  mout[2] = (zx - w_y) + (zx - w_y);
  mout[4] = (yx - w_z) + (yx - w_z);
  mout[5] = 1.0f - ((zz + xx) + (zz + xx));
  mout[12] = 0.0f;
  mout[13] = 0.0f;
  mout[14] = 0.0f;
  *(int *)&mout[3] = 3;
  mout[6] = (x_w + zy) + (x_w + zy);
  mout[8] = (w_y + zx) + (w_y + zx);
  mout[9] = (zy - x_w) + (zy - x_w);
  mout[10] = 1.0f - ((yy + xx) + (yy + xx));
  d_x = qb[4] - lp.f[0];
  d_y = qb[5] - lp.f[1];
  d_z = qb[6] - lp.f[2];
  px = (d_z * m[8] + d_y * m[4]) + d_x * m[0];
  py = (d_z * m[9] + d_y * m[5]) + d_x * m[1];
  pz = (d_z * m[10] + d_y * m[6]) + d_x * m[2];
  mout[12] = m[12] + px;
  mout[13] = m[13] + py;
  mout[14] = m[14] + pz;
  return body;
}
