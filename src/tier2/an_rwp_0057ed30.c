#include "mfl_tier2.h"
#include "rwp_common.h"
#include <math.h>

extern void rwp_batch_0057f410(char *dst, char *lhs, char *rhs);

/* Output array of 0x30 / 0x20 byte records: base pointer and the number of records written. */
typedef struct MflRecArray {
  char *base;
  unsigned count;   /* 0x4 */
} MflRecArray;

/* Step setup of the batched rigid body solver, for the bodies listed in `list`.
   Pass 1 (per body b): iinv[i] = the 0x30 byte world space inverse inertia record of the body: for a body with flag bit 1 (flags[4 * b]) the
   3x3 matrix (rows 16 bytes apart) that 57f410 builds from the body's inertia (inertia, 48 bytes) and orientation (orient, 64 bytes), else the scalar at
   +0x2c on the diagonal; the word at +0xc (inverse mass) is copied.  For a flagged body the gyroscopic term is added to its angular velocity
   (vel_in + 32 * b + 0x10, written to vel + 32 * b + 0x10): w = R^T u with R from 57f410 (a4, orient), c = u x w, and when |c|^2 is not tiny a
   correction step: r = sqrt((d + dk) / dk) from d = |c| |u|^2 dt^2 / 4 and dk = (u . w) / 2, k = (1 - r) / dt, c = (c + w k) / r.
   Pass 2 (per body): out[i] holds the linear part s^2 * vel + (1 / dt) * vel_in and the angular part (1 / dt) * vel_in + M^2 * vel(angular), where
   M is the 3x3 of the body's iinv record and s its inverse mass word.  Both counts are written back (the unflagged-body parameters are unused). */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
void rwp_batch_0057ed30(float *vel, int a1, MflRecArray *iinv, MflRecArray *out, char *a4, int a5, char *inertia, MflIndexList *list,
                        char *orient, int a9, char *vel_in, int a11, unsigned char *flags, int a13, float dt)
{
  unsigned i;
  int b, off, oc;
  float *u, *w, *m, *o, *vw;
  float R[12];
  float N[12];
  float rtu_x, rtu_y, rtu_z, v0, v1, v2;
  float uxw_y, uxw_z, inv, s;
  volatile float len2, d, r, k, ir;
  volatile float t0, t1, t2, p96;
  double acc, uxw_x, dk, inv_mass_sq, n20, m40;

  off = 0;
  for (i = 0; i != list->count; i++) {
    b = list->items[i];
    if (flags[b * 4] & 2) {
      rwp_batch_0057f410(iinv->base + off, inertia + b * 48, orient + b * 64);
      *(int *)(iinv->base + off + 0xc) = *(int *)(inertia + b * 48 + 0xc);
    } else {
      s = *(float *)(inertia + b * 48 + 0x2c);
      *(float *)(iinv->base + off) = s;
      *(float *)(iinv->base + off + 4) = 0.0f;
      *(float *)(iinv->base + off + 8) = 0.0f;
      *(float *)(iinv->base + off + 0x10) = 0.0f;
      *(float *)(iinv->base + off + 0x14) = s;
      *(float *)(iinv->base + off + 0x18) = 0.0f;
      *(float *)(iinv->base + off + 0x20) = 0.0f;
      *(float *)(iinv->base + off + 0x24) = 0.0f;
      *(float *)(iinv->base + off + 0x28) = s;
      *(int *)(iinv->base + off + 0xc) = *(int *)(inertia + b * 48 + 0xc);
    }
    if (flags[b * 4] & 2) {
      rwp_batch_0057f410((char *)R, a4 + b * 48, orient + b * 64);
      u = (float *)(vel_in + b * 32);
      acc = R[8] * u[6];
      acc = acc + R[4] * u[5];
      acc = acc + R[0] * u[4];
      rtu_x = acc;
      v0 = rtu_x;
      acc = R[9] * u[6];
      acc = acc + R[5] * u[5];
      acc = acc + R[1] * u[4];
      rtu_y = acc;
      v1 = rtu_y;
      acc = R[10] * u[6];
      acc = acc + R[6] * u[5];
      acc = acc + R[2] * u[4];
      rtu_z = acc;
      v2 = rtu_z;
      uxw_x = u[6] * v1 - u[5] * v2;
      uxw_y = v2 * u[4] - u[6] * v0;
      uxw_z = u[5] * v0 - v1 * u[4];
      acc = uxw_z * uxw_z;
      acc = acc + uxw_y * uxw_y;
      acc = acc + uxw_x * uxw_x;
      len2 = (float)acc;
      if ((float)len2 > 1.17549435e-38f) {
        acc = u[4] * u[4];
        acc = acc + u[5] * u[5];
        acc = acc + u[6] * u[6];
        acc = sqrt(len2) * acc;
        acc = acc * dt;
        acc = acc * dt;
        d = (float)(acc * 0.25f);
        w = (float *)((char *)vel + b * 32 + 0x10);
        if (d > 1.17549435e-38f) {
          dk = u[5] * v1;
          dk = dk + v0 * u[4];
          dk = dk + u[6] * v2;
          dk = dk * 0.5f;
          r = (float)sqrt((d + dk) / dk);
          k = (float)((1.0f - r) / dt);
          t1 = rtu_y * k + uxw_y;
          t2 = rtu_z * k + uxw_z;
          ir = (float)(1.0f / r);
          uxw_y = t1 * ir;
          uxw_z = t2 * ir;
          w[0] = (uxw_x + rtu_x * k) * ir + w[0];
        } else {
          w[0] = uxw_x + w[0];
        }
        w[1] = uxw_y + w[1];
        w[2] = uxw_z + w[2];
      }
    }
    off += 0x30;
  }
  inv = 1.0f / dt;
  iinv->count = list->count;
  oc = 0;
  off = 0;
  for (i = 0; i != list->count; i++) {
    b = list->items[i];
    m = (float *)(iinv->base + off);
    t0 = m[0] * m[0];
    t1 = m[1] * m[0];
    t2 = m[2] * m[0];
    t0 = t0 + m[4] * m[1];
    t1 = m[1] * m[5] + t1;
    t2 = m[1] * m[6] + t2;
    t0 = t0 + m[2] * m[8];
    N[0] = t0;
    t1 = m[9] * m[2] + t1;
    N[1] = t1;
    t2 = m[2] * m[10] + t2;
    N[2] = t2;
    m40 = m[4] * m[0];
    t2 = m[4] * m[2];
    t0 = m[4] * m[5] + m40;
    t0 = m[8] * m[6] + t0;
    p96 = m[9] * m[6];
    t1 = m[5] * m[5] + m[4] * m[1] + p96;
    t2 = m[6] * m[10] + (m[6] * m[5] + t2);
    N[4] = t0;
    N[5] = t1;
    N[6] = t2;
    t0 = m[8] * m[0];
    t1 = m[1] * m[8];
    t1 = m[9] * m[5] + t1;
    t2 = m[2] * m[8] + p96;
    t1 = m[9] * m[10] + t1;
    t2 = m[10] * m[10] + t2;
    n20 = (m[9] * m[4] + t0) + m[8] * m[10];
    inv_mass_sq = m[3] * m[3];
    o = (float *)(out->base + oc);
    vw = (float *)((char *)vel + b * 32);
    u = (float *)(vel_in + b * 32);
    o[0] = inv_mass_sq * vw[0] + inv * u[0];
    o[1] = inv_mass_sq * vw[1] + inv * u[1];
    o[2] = inv_mass_sq * vw[2] + inv * u[2];
    o[4] = inv * u[4];
    o[5] = inv * u[5];
    o[6] = inv * u[6];
    o[4] = N[0] * vw[4] + o[4];
    o[5] = N[1] * vw[4] + o[5];
    o[6] = N[2] * vw[4] + o[6];
    o[4] = N[4] * vw[5] + o[4];
    o[5] = N[5] * vw[5] + o[5];
    o[6] = N[6] * vw[5] + o[6];
    o[4] = n20 * vw[6] + o[4];
    o[5] = t1 * vw[6] + o[5];
    o[6] = t2 * vw[6] + o[6];
    oc += 0x20;
    off += 0x30;
  }
  out->count = list->count;
}
