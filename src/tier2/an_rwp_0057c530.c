#include "mfl_tier2.h"
/* body set of the Rwp batch: parallel arrays indexed by the body number */
typedef struct RwpQuat { float x, y, z, w; } RwpQuat;
typedef struct RwpBodyVel { RwV3d lin; float pad0; RwV3d ang; float pad1; } RwpBodyVel;
typedef struct RwpBodySet {
  RwMatrix *matrix;     /* 0x00 */
  int pad04;
  RwpBodyVel *vel;      /* 0x08 */
  int pad0c;
  RwpQuat *quat;        /* 0x10 */
  int pad14;
  unsigned int *flags;  /* 0x18, bit 0 = rotation constrained to an axis */
  int pad1c[13];
  RwpQuat *axis;        /* 0x50 */
} RwpBodySet;
/*0057c230*/ extern int rwp_batch_0057c230(RwpQuat *quat, RwpQuat *axis, float angle);
/*0057c390*/ extern int rwp_batch_0057c390(RwpQuat *quat, RwV3d *ang_vel, float dt);
/*004d3cc0*/ extern int RwMatrixUpdate(RwMatrix *m);

/* One integration step of body `i`: removes the angular velocity component along the constraint axis (and
   rotates the orientation by it), integrates the orientation and the position over dt, and rebuilds the body's
   rotation matrix from the unit quaternion (R from q with s = 2 / |q|^2). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_batch_0057c530(RwpBodySet *b, int i, float dt)
{
  RwMatrix *m = b->matrix + i;
  RwpBodyVel *vel = b->vel + i;
  RwpQuat *q = b->quat + i;
  RwV3d w = vel->ang;
  RwV3d np;
  volatile float d;
  float nd, xx2, yy2, zz2, yz2, wx2, wy2, wz2;
  double s, sx, sz, xz2, xy2, acc, t_;
  float sy;

  if (b->flags[i] & 1) {
    RwpQuat *ax = b->axis + i;
    acc = ax->y * w.y;
    acc = acc + w.x * ax->x;
    acc = acc + ax->z * w.z;
    d = acc;      /* stored to the frame as a float (the original spills here: overflow / denormals round) */
    acc = acc * dt;
    rwp_batch_0057c230(q, ax, acc);
    nd = -d;
    w.x = nd * ax->x + w.x;
    w.y = ax->y * nd + w.y;
    w.z = ax->z * nd + w.z;
  }
  rwp_batch_0057c390(q, &w, dt);
  np.x = dt * vel->lin.x + m->pos.x;
  np.y = dt * vel->lin.y + m->pos.y;
  np.z = dt * vel->lin.z + m->pos.z;
  acc = q->x * q->x;
  acc = acc + q->y * q->y;
  acc = acc + q->z * q->z;
  acc = acc + q->w * q->w;
  s = 2.0f / acc;
  sx = s * q->x;
  sy = q->y * s;
  sz = q->z * s;
  wx2 = q->w * sx;
  wy2 = q->w * sy;
  wz2 = q->w * sz;
  xx2 = sx * q->x;
  yy2 = sy * q->y;
  zz2 = sz * q->z;
  yz2 = sz * q->y;
  xz2 = sx * q->z;
  xy2 = sy * q->x;
  m->pos = np;
  m->flags = 3;
  m->right.x = 1.0f - (zz2 + yy2);
  m->right.y = wz2 + xy2;
  m->right.z = xz2 - wy2;
  m->up.x = xy2 - wz2;
  m->up.y = 1.0f - (zz2 + xx2);
  m->up.z = yz2 + wx2;
  m->at.x = xz2 + wy2;
  m->at.y = yz2 - wx2;
  m->at.z = 1.0f - (yy2 + xx2);
  return RwMatrixUpdate(m);
}
