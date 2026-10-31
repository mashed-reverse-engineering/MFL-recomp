#include "mfl_tier2.h"

extern void rwp_batch_0057c230(float *q, float *axis, float angle);
extern void rwp_batch_0057c390(float *q, float *omega, float dt);
extern RwMatrix *RwMatrixUpdate(RwMatrix *m);

typedef struct MflV3 {
  float v[3];
} MflV3;
/* A list of body indices. */
typedef struct MflIndexList {
  int *items;
  unsigned int count;   /* 0x4 */
} MflIndexList;

/* Integrates the orientation and position of the listed bodies over dt.  For every index in the list: the body's 32 byte record in
   recs holds its velocity (0x0) and angular velocity (0x10); when the flag byte of the body (flags[4 * index], bit 0) is set the angular velocity is
   first split along the body's axis (axes[index]): the quaternion quats[index] is rotated about the axis by (omega . axis) * dt by 57c230 and
   that component is removed from omega.  The quaternion is then integrated by omega (57c390) and written as the rotation of the 4x4 matrix
   mats[index] (flags 3), whose position becomes velocity * dt added to the position it had. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_batch_0057c750(float *mats, int a2, float *quats, int a4, float *recs, MflIndexList *list, float *axes, int a8,
                        unsigned char *flags, int a10, float dt)
{
  unsigned int i;
  int idx;
  float *rec, *ax, *q, *m;
  float t;
  float b[3];
  MflV3 oldpos;
  double s, xs, zs, d27, d28, dot;
  float ys, xw, yw, zw, xx, yy, zz, yz;

  for (i = 0; i < list->count; i++) {
    idx = list->items[i];
    rec = recs + idx * 8;
    b[0] = rec[4];
    b[1] = rec[5];
    b[2] = rec[6];
    if (flags[idx * 4] & 1) {
      ax = axes + idx * 4;
      dot = ax[1] * b[1];
      dot = dot + ax[0] * b[0];
      dot = dot + ax[2] * b[2];
      t = dot;
      rwp_batch_0057c230(quats + idx * 4, ax, dot * dt);
      b[0] = b[0] - t * ax[0];
      b[1] = b[1] - t * ax[1];
      b[2] = b[2] - t * ax[2];
    }
    q = quats + idx * 4;
    rwp_batch_0057c390(q, b, dt);
    m = mats + idx * 16;
    oldpos = *(MflV3 *)(m + 12);
    s = q[0] * q[0];
    s = s + q[1] * q[1];
    s = s + q[2] * q[2];
    s = s + q[3] * q[3];
    s = 2.0f / s;
    xs = q[0] * s;
    ys = q[1] * s;
    zs = q[2] * s;
    xw = q[3] * xs;
    yw = q[3] * ys;
    zw = q[3] * zs;
    xx = xs * q[0];
    yy = ys * q[1];
    zz = zs * q[2];
    yz = zs * q[1];
    d27 = xs * q[2];
    d28 = ys * q[0];
    *(int *)(m + 3) = 3;
    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[0] = 1.0f - (zz + yy);
    m[1] = zw + d28;
    m[2] = d27 - yw;
    m[4] = d28 - zw;
    m[5] = 1.0f - (zz + xx);
    m[6] = yz + xw;
    m[8] = d27 + yw;
    m[9] = yz - xw;
    m[10] = 1.0f - (yy + xx);
    m[12] = rec[0] * dt;
    m[13] = rec[1] * dt;
    m[14] = rec[2] * dt;
    m[12] = m[12] + oldpos.v[0];
    m[13] = m[13] + oldpos.v[1];
    m[14] = m[14] + oldpos.v[2];
    RwMatrixUpdate((RwMatrix *)m);
  }
}
