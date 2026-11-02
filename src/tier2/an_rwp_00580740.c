/* rwp_constraint_linear_row_axis (0x00580740): joint constraint builder: linear row along a world axis */
#include "mfl_tier2.h"
#include "rwp_common.h"
#include <math.h>

/* Joint constraint builder: for one joint (pair) between two bodies, appends the solver rows (27 words each, see MflRow) that hold the
   joint together or drive it: linear axes, angular limits, motors and soft limits.  The solver state counts the rows in s->count. */

/* Joint description (the part used here). */
typedef struct MflJointDef {
  int pad00;
  float frameA[7];   /* 0x4: orientation quaternion + offset of the joint frame in body A */
  float frameB[7];   /* 0x20 */
  unsigned flags;   /* 0x3c: 8 / 0x10 / 0x20 = fixed linear axis 0..2, 1 / 2 / 4 = fixed angular axis 0..2 */
  unsigned char dist_flags;   /* 0x40: bit 0 = twist limit hi, 0x38 = distance limit hi */
  unsigned char pad41[3];
  float dist_hi;   /* 0x44 */
  float twist_hi;   /* 0x48 */
  float swing_hi_a;   /* 0x4c */
  float swing_hi_b;   /* 0x50 */
  unsigned char lo_flags;   /* 0x54 */
  unsigned char pad55[3];
  float dist_lo;   /* 0x58 */
  float twist_lo;   /* 0x5c */
  float swing_lo_a;   /* 0x60 */
  float swing_lo_b;   /* 0x64 */
  union { float f; int bits; } dist_hi_stiff;   /* 0x68: 0x7f7fffff = unlimited */
  float dist_lo_soft;   /* 0x6c */
  int pad70;
  float twist_hi_stiff;   /* 0x74 */
  float twist_hi_soft;   /* 0x78 */
  int pad7c;
  float swing_stiff;   /* 0x80 */
  float swing_soft;   /* 0x84 */
  int pad88;
  unsigned drive;   /* 0x8c: bits m / m<<8 / m<<16 = target / velocity / force limit for each drive channel */
  float lin_stiff;   /* 0x90 */
  float lin_soft;   /* 0x94 */
  float lin_force;   /* 0x98 */
  float twistx_stiff;   /* 0x9c */
  float twistx_soft;   /* 0xa0 */
  float twistx_force;   /* 0xa4 */
  float rot_stiff;   /* 0xa8 */
  float rot_soft;   /* 0xac */
  float rot_force;   /* 0xb0 */
} MflJointDef;

typedef struct MflJoint {
  unsigned char pad00[0x4c];
  MflJointDef *def;   /* 0x4c */
  void *handleA[2];   /* 0x50: body handle (object, id) */
  void *handleB[2];   /* 0x58 */
  float rot[4];   /* 0x60: x y z w of the drive orientation */
  float target[3];   /* 0x70: position drive target */
  float velocity[3];   /* 0x7c: velocity drive target */
  unsigned char pad88[4];
  float spin[3];   /* 0x8c: angular drive velocity */
} MflJoint;

typedef struct MflBodyRec {
  unsigned char pad00[0x20];
  short id;   /* 0x20 */
} MflBodyRec;

extern MflRow g_rwp_constraint_padding;
extern MflBodyRec *rwp_shape_frame_relative_to_body(void *handle, float *frame, float *q_out, float **pos_out, float *m_out);
extern void rwp_quat_combine_products(float *q, float *r, float *prod, float *half);
extern void rwp_constraint_store(MflSolver *s, MflRow *row);
extern void rwp_constraints_0057f750(MflSolver *s);
extern float rwp_cylinder_0056fd20(float *out, float *in);

#define TN ((sq * rel0 - i5 * 0.5f) / t1)
#define TW_E (2.0f * ((1.0f - lim) * q[3]))
#define TW_X (lim * q[0] * -2.0f)
/* the definition is re-read at every use, like the original (a callee may rewrite the pointer; the compiler would otherwise load it once) */
#define JD (p->def)
#define FMAX 3.4028235e+38f

/* ---- macros shared by the row builders (functions would round their float arguments; diffemu cannot call static functions) ---- */
/* spring rows: f = 1 / (dt * stiff + soft), cfm from f and the step h, erp from f * stiff * dt */
#define CFM(row, f) ((row).cfm = ((f) > h * 0.1f) ? h * -0.9f : (f) - h)
/* angular Jacobian of the row: side 1 gets v, side 2 gets -v */
#define ANG(row, x, y, z) do { \
    if (has_a) { (row).ang1[0] = (x); (row).ang1[1] = (y); (row).ang1[2] = (z); } \
    if (has_b) { (row).ang2[0] = -(x); (row).ang2[1] = -(y); (row).ang2[2] = -(z); } } while (0)
/* linear row along the world axis r (a row of B's matrix): side 1 moves against r at lever arm ra, side 2 with r at lever arm rb */
#define LIN(row, r, ra, rb) do { \
    if (has_a) { \
      (row).lin1[0] = -(r)[0]; (row).lin1[1] = -(r)[1]; (row).lin1[2] = -(r)[2]; \
      (row).ang1[0] = (r)[1] * (ra)[2] - (r)[2] * (ra)[1]; \
      (row).ang1[1] = (r)[2] * (ra)[0] - (r)[0] * (ra)[2]; \
      (row).ang1[2] = (r)[0] * (ra)[1] - (r)[1] * (ra)[0]; } \
    if (has_b) { \
      (row).lin2[0] = (r)[0]; (row).lin2[1] = (r)[1]; (row).lin2[2] = (r)[2]; \
      (row).ang2[0] = (r)[2] * (rb)[1] - (r)[1] * (rb)[2]; \
      (row).ang2[1] = (r)[0] * (rb)[2] - (r)[2] * (rb)[0]; \
      (row).ang2[2] = (r)[1] * (rb)[0] - (r)[0] * (rb)[1]; } } while (0)

/* TIER2: unvalidated 2026-11-01 authority=diffemu */
void rwp_constraint_linear_row_axis(MflSolver *s, MflJoint *p, float rate, float dt)
{
  MflRow row, row2, row3;
  MflBodyRec *body_a, *body_b;
  int has_a, has_b, rows, side, ax;
  unsigned mask, u, nm;
  float h, f, k, e, dist, m, inv;
  float qa[4], qb[4], q[4], half[12];
  float *pos_a, *pos_b, *r;
  float ma[16], mb[16];
  float ra[3], rb[3], d[3], ec[3], dir[3], v[3], c0[3], c1[3], c2[3], g[3], dq[4], pr[3], jm[9];
  float xx, yy, zz, yw, rel0, rel4, rel8, qx, qy, qz, qw, wx, wy, wz;
  volatile float tp0, tp1, p1, p2, u1, u2, ka, kb, sq;
  double w0, w1, w2, tn;
  float t1, t2, s6, s8, i5, i9, r4, r7, lim, sa, sb, ca, cb, cc, cd, sw1, sw2, ta, tb, m1, m2, s1, ia, ib, x, y;

  h = rate * dt;
  rows = 0;
  body_a = rwp_shape_frame_relative_to_body(&p->handleA, p->def->frameA, qa, &pos_a, ma);
  body_b = rwp_shape_frame_relative_to_body(&p->handleB, p->def->frameB, qb, &pos_b, mb);
  has_a = body_a != 0;
  has_b = body_b != 0;
  /* lever arms from the body positions to the joint anchor, and the anchor distance vector */
  ra[0] = ma[12] - pos_a[0];
  ra[1] = ma[13] - pos_a[1];
  ra[2] = ma[14] - pos_a[2];
  rb[0] = ma[12] - pos_b[0];
  rb[1] = ma[13] - pos_b[1];
  rb[2] = ma[14] - pos_b[2];
  d[0] = ma[12] - mb[12];
  d[1] = ma[13] - mb[13];
  d[2] = ma[14] - mb[14];
  s->words[s->cursor] = 0;
  s->count = s->count + 1;
  s->pairs[s->cursor][0] = body_a ? (int)body_a->id : -1;
  s->pairs[s->cursor][1] = body_b ? (int)body_b->id : -1;

  /* relative orientation q of the two frames; (rel0, rel4, rel8) = first column of its rotation matrix; ec = anchor error in B's axes */
  rwp_quat_combine_products(qa, qb, q, half);
  xx = q[0] * q[0];
  yy = q[1] * q[1];
  zz = q[2] * q[2];
  yw = q[3] * q[1];
  rel0 = 1.0f - (zz + yy + zz + yy);
  e = q[1] * q[0] - q[3] * q[2];
  rel4 = e + e;
  e = yw + q[2] * q[0];
  rel8 = e + e;
  ec[0] = mb[2] * d[2] + mb[0] * d[0] + mb[1] * d[1];
  ec[1] = mb[6] * d[2] + mb[4] * d[0] + mb[5] * d[1];
  ec[2] = mb[10] * d[2] + mb[8] * d[0] + mb[9] * d[1];

  /* fixed linear axes: one row along each of B's axes */
  mask = 8;
  for (ax = 0; ax < 3; ax++, mask *= 2) {
    r = &mb[4 * ax];
    if (JD->flags & mask) {
      e = ec[ax];
      row = g_rwp_constraint_padding;
      LIN(row, r, ra, rb);
      row.lo = -FMAX;
      row.hi = FMAX;
      row.lo2 = -FMAX;
      row.hi2 = FMAX;
      row.rhs = -e;
      row.tol = 0x3f4ccccd;
      rwp_constraint_store(s, &row);
      rows++;
    }
  }

  /* fixed angular axes: x about the relative twist, y / z about the swing axes */
  if (JD->flags & 1) {
    row = g_rwp_constraint_padding;
    ANG(row, half[3], half[4], half[5]);
    row.lo = -FMAX;
    row.hi = FMAX;
    row.lo2 = -FMAX;
    row.hi2 = FMAX;
    row.rhs = -q[0];
    row.tol = 0x3f4ccccd;
    rwp_constraint_store(s, &row);
    rows++;
  }
  u = JD->flags & 6;
  if (u == 2) {
    v[0] = ma[1] * mb[6] - ma[2] * mb[5];
    v[1] = ma[2] * mb[4] - ma[0] * mb[6];
    v[2] = ma[0] * mb[5] - ma[1] * mb[4];
    row = g_rwp_constraint_padding;
    row.rhs = rel4;
    ANG(row, v[0], v[1], v[2]);
    row.lo = -FMAX;
    row.hi = FMAX;
    row.lo2 = -FMAX;
    row.hi2 = FMAX;
    row.tol = 0x3f4ccccd;
    rwp_constraint_store(s, &row);
    rows++;
  } else if (u == 4) {
    v[0] = ma[1] * mb[10] - ma[2] * mb[9];
    v[1] = ma[2] * mb[8] - ma[0] * mb[10];
    v[2] = ma[0] * mb[9] - ma[1] * mb[8];
    row = g_rwp_constraint_padding;
    row.rhs = rel8;
    ANG(row, v[0], v[1], v[2]);
    row.lo = -FMAX;
    row.hi = FMAX;
    row.lo2 = -FMAX;
    row.hi2 = FMAX;
    row.tol = 0x3f4ccccd;
    rwp_constraint_store(s, &row);
    rows++;
  } else if (u == 6) {
    row = g_rwp_constraint_padding;
    row2 = g_rwp_constraint_padding;
    ANG(row, half[6], half[7], half[8]);
    ANG(row2, half[9], half[10], half[11]);
    row.lo = row2.lo = -FMAX;
    row.hi = row2.hi = FMAX;
    row.lo2 = row2.lo2 = -FMAX;
    row.hi2 = row2.hi2 = FMAX;
    row.tol = row2.tol = 0x3f4ccccd;
    row.rhs = -q[1];
    row2.rhs = -q[2];
    rwp_constraint_store(s, &row);
    rwp_constraint_store(s, &row2);
    rows += 2;
  }

  /* distance limit along the anchor direction: side 2 = beyond the upper limit, side 1 = beyond the lower one */
  dist = rwp_cylinder_0056fd20(dir, d);
  side = 0;
  e = 0;
  if ((JD->dist_flags & 0x38) && (e = dist - JD->dist_hi, e > 0)) side = 2;
  else if ((JD->lo_flags & 0x38) && (e = dist - JD->dist_lo, 0 < e)) side = 1;
  if (side) {
    row = g_rwp_constraint_padding;
    c0[1] = dir[0] * ra[2] - dir[2] * ra[0];
    c0[2] = dir[1] * ra[0] - dir[0] * ra[1];
    c1[0] = dir[2] * rb[1] - dir[1] * rb[2];
    c1[1] = dir[0] * rb[2] - dir[2] * rb[0];
    c1[2] = dir[1] * rb[0] - dir[0] * rb[1];
    if (has_a) {
      row.lin1[0] = -dir[0];
      row.lin1[1] = -dir[1];
      row.lin1[2] = -dir[2];
      row.ang1[0] = -(dir[2] * ra[1] - dir[1] * ra[2]);
      row.ang1[1] = -c0[1];
      row.ang1[2] = -c0[2];
    }
    if (has_b) {
      row.lin2[0] = dir[0];
      row.lin2[1] = dir[1];
      row.lin2[2] = dir[2];
      row.ang2[0] = c1[0];
      row.ang2[1] = c1[1];
      row.ang2[2] = c1[2];
    }
    if (side != 2 || (JD->lo_flags & 0x38) == 0 || !(dist - JD->dist_lo > 0) || JD->dist_hi_stiff.bits == 0x7f7fffff)
      row.lo = 0;
    else
      row.lo = (JD->dist_hi - JD->dist_lo) * JD->dist_hi_stiff.f;
    row.hi = FMAX;
    row.lo2 = 0;
    row.hi2 = FMAX;
    row.rhs = -e;
    if (side == 1) {
      f = dt * JD->dist_hi_stiff.f + JD->dist_lo_soft;
      if (!(f > 0)) goto dist_done;
      f = 1.0f / f;
      CFM(row, f);
      row.erp = f * dt * JD->dist_hi_stiff.f;
    }
    row.tol = 0x3f4ccccd;
    rwp_constraint_store(s, &row);
    rows++;
  }
dist_done:

  /* drives on the linear axes that are not fixed: target position / velocity, spring (stiff / soft) and force limit */
  mask = 8;
  for (ax = 0; ax < 3; ax++, mask *= 2) {
    r = &mb[4 * ax];
    u = JD->drive;
    if ((JD->flags & mask) == 0 && (u & (mask * 0x101)) != 0) {
      row = g_rwp_constraint_padding;
      if (u & mask) row.rhs = p->target[ax] - ec[ax];
      if (u & (mask << 8)) row.vel = -p->velocity[ax];
      if ((u & (mask << 16)) == 0) {
        row.hi = FMAX;
        row.lo = -FMAX;
      } else {
        row.hi = JD->lin_force;
        row.lo = -JD->lin_force;
      }
      LIN(row, r, ra, rb);
      f = dt * JD->lin_stiff + JD->lin_soft;
      if (0 < f) {
        f = 1.0f / f;
        CFM(row, f);
        row.tol = 0x3f4ccccd;
        row.erp = f * JD->lin_stiff * dt;
        rwp_constraint_store(s, &row);
        rows++;
      }
    }
  }

  /* angular drives */
  u = JD->drive;
  if (u & 0x707) {
    qx = p->rot[0];
    qy = p->rot[1];
    qz = p->rot[2];
    qw = p->rot[3];
    wx = p->spin[0];
    wy = p->spin[1];
    wz = p->spin[2];
    dq[0] = ((-(qx * wx) - qy * wy) - qz * wz) * 0.5f;
    dq[1] = ((wx * qw + qz * wy) - qy * wz) * 0.5f;
    dq[2] = (qx * wz + (wy * qw - qz * wx)) * 0.5f;
    dq[3] = (wz * qw + (qy * wx - qx * wy)) * 0.5f;
    if ((JD->flags & 7) == 0 && (u & 0x7000000) != 0) {
      /* all three angular axes free: three rows from the drive orientation */
      c0[0] = qw * q[0] + qx * q[3] + (qz * q[1] - qy * q[2]);
      c0[1] = qw * q[1] + qy * q[3] + (qx * q[2] - qz * q[0]);
      c0[2] = qw * q[2] + qz * q[3] + (qy * q[0] - qx * q[1]);
      g[0] = dq[0] * q[0] + dq[1] * q[3] + (dq[3] * q[1] - dq[2] * q[2]);
      g[1] = dq[0] * q[1] + dq[2] * q[3] + (dq[1] * q[2] - dq[3] * q[0]);
      g[2] = dq[0] * q[2] + dq[3] * q[3] + (dq[2] * q[0] - dq[1] * q[1]);
      jm[0] = (half[6] * qz + half[0] * qx + half[3] * qw) - half[9] * qy;
      jm[1] = (half[7] * qz + half[1] * qx + half[4] * qw) - half[10] * qy;
      jm[2] = (half[8] * qz + half[2] * qx + half[5] * qw) - half[11] * qy;
      jm[3] = half[9] * qx + qw * half[6] + (half[0] * qy - half[3] * qz);
      jm[4] = half[10] * qx + qw * half[7] + (half[1] * qy - half[4] * qz);
      jm[5] = half[11] * qx + qw * half[8] + (half[2] * qy - half[5] * qz);
      jm[6] = qw * half[9] + ((half[0] * qz + half[3] * qy) - half[6] * qx);
      jm[7] = qw * half[10] + ((half[1] * qz + half[4] * qy) - half[7] * qx);
      jm[8] = qw * half[11] + ((half[2] * qz + half[5] * qy) - half[8] * qx);
      row = g_rwp_constraint_padding;
      row2 = g_rwp_constraint_padding;
      row3 = g_rwp_constraint_padding;
      if (u & 7) {
        row.rhs = -c0[0];
        row2.rhs = -c0[1];
        row3.rhs = -c0[2];
      }
      if (u & 0x700) {
        row.vel = g[0];
        row2.vel = g[1];
        row3.vel = g[2];
      }
      if ((u & 0x70000) == 0) {
        row3.lo = -FMAX;
        row3.hi = FMAX;
      } else {
        row3.hi = JD->rot_force;
        row3.lo = -row3.hi;
      }
      row.lo2 = row.hi2 = row2.lo2 = row2.hi2 = row3.lo2 = row3.hi2 = 0;
      ANG(row, jm[0], jm[1], jm[2]);
      ANG(row2, jm[3], jm[4], jm[5]);
      ANG(row3, jm[6], jm[7], jm[8]);
      f = dt * JD->rot_stiff + JD->rot_soft;
      row.lo = row2.lo = row3.lo;
      row.hi = row2.hi = row3.hi;
      if (f > 0) {
        f = 1.0f / f;
        CFM(row, f);
        row.tol = row2.tol = row3.tol = 0x3f4ccccd;
        row.erp = f * JD->rot_stiff * dt;
        row2.cfm = row3.cfm = row.cfm;
        row2.erp = row3.erp = row.erp;
        rwp_constraint_store(s, &row);
        rwp_constraint_store(s, &row2);
        rwp_constraint_store(s, &row3);
        rows += 3;
      }
    } else {
      if ((JD->flags & 1) == 0 && (u & 0x101) != 0) {
        /* twist about x */
        t1 = rel0 + 1.0f;
        row = g_rwp_constraint_padding;
        ia = 1.0f / sqrt(t1 * 0.5f);
        ib = 1.0f / sqrt(qx * qx + qw * qw);
        k = ib * ia;
        e = (q[3] * qw - q[0] * qx) * k;
        if (u & 1) row.rhs = (q[0] * qw + q[3] * qx) * k;
        if (u & 0x100) row.vel = -((dq[1] * qw - dq[0] * qx) * e * ib);
        if ((u & 0x10000) == 0) {
          row.lo = -FMAX;
          row.hi = FMAX;
        } else {
          row.hi = JD->twistx_force;
          row.lo = -JD->twistx_force;
        }
        e = e * ia;
        x = q[0] * e;
        row.lo2 = row.hi2 = 0;
        y = -(e * q[3]);
        tp0 = half[0] * x;   /* frame floats in the original, like in the twist limit */
        tp1 = half[1] * x;
        v[0] = half[3] * y + tp0;
        v[1] = half[4] * y + tp1;
        v[2] = y * half[5] + x * half[2];
        ANG(row, v[0], v[1], v[2]);
        f = dt * JD->twistx_stiff + JD->twistx_soft;
        if (0 < f) {
          f = 1.0f / f;
          CFM(row, f);
          row.tol = 0x3f4ccccd;
          row.erp = f * JD->twistx_stiff * dt;
          rwp_constraint_store(s, &row);
          rows++;
        }
      }
      nm = ~(JD->flags & 6);
      if (nm != 0 && (JD->drive & 0x606) != 0) {
        /* swing drives about y and z */
        t1 = rel0 + 1.0f;
        qz = p->rot[2];
        t2 = t1 + t1;
        s6 = (p->rot[0] * p->rot[0] + p->rot[3] * p->rot[3]) * 4.0f;
        r4 = sqrt(t2);
        r7 = sqrt(s6);
        i5 = 1.0f / (r4 + t1);
        s8 = s6 * 0.5f;
        i9 = 1.0f / (r7 + s8);
        ca = (1.0f + r4) / ((t2 + t2 + (rel0 + 3.0f) * r4) * t1);
        cb = (r7 + 1.0f) / ((s6 + s6 + (s8 + 2.0f) * r7) * s8);
        e = qz * qz + qy * qy;
        pr[0] = 1.0f - (e + e);
        pr[1] = qw * qz + qy * qx;
        pr[1] = pr[1] + pr[1];
        pr[2] = qz * qx - qw * qy;
        pr[2] = pr[2] + pr[2];
        dq[1] = pr[2] * wy - pr[1] * wz;
        dq[2] = wz * pr[0] - pr[2] * wx;
        dq[3] = pr[1] * wx - wy * pr[0];
        c0[0] = ma[1] * mb[2] - ma[2] * mb[1];
        c0[1] = ma[2] * mb[0] - ma[0] * mb[2];
        c0[2] = ma[0] * mb[1] - ma[1] * mb[0];
        c1[0] = ma[1] * mb[6] - ma[2] * mb[5];
        c1[1] = ma[2] * mb[4] - ma[0] * mb[6];
        c1[2] = ma[0] * mb[5] - ma[1] * mb[4];
        c2[0] = ma[1] * mb[10] - ma[2] * mb[9];
        c2[1] = ma[2] * mb[8] - ma[0] * mb[10];
        c2[2] = ma[0] * mb[9] - ma[1] * mb[8];
        if ((nm & 2) != 0 && 0.01f < t2) {
          u = JD->drive;
          row = g_rwp_constraint_padding;
          if (u & 2) row.rhs = i5 * rel4 - pr[1] * i9;
          if (u & 0x200) row.vel = dq[2] * i9 - dq[1] * pr[1] * cb;
          f = ca * rel4;
          c1[0] = c0[0] * f + c1[0] * i5;
          c1[1] = c0[1] * f + c1[1] * i5;
          c1[2] = c0[2] * f + c1[2] * i5;
          ANG(row, c1[0], c1[1], c1[2]);
          if ((u & 0x20000) == 0) {
            row.lo = -FMAX;
            row.hi = FMAX;
          } else {
            row.hi = JD->rot_force;
            row.lo = -JD->rot_force;
          }
          row.lo2 = row.hi2 = 0;
          f = dt * JD->rot_stiff + JD->rot_soft;
          if (0 < f) {
            f = 1.0f / f;
            CFM(row, f);
            row.tol = 0x3f4ccccd;
            row.erp = f * JD->rot_stiff * dt;
            rwp_constraint_store(s, &row);
            rows++;
          }
        }
        if ((nm & 4) != 0 && 0.01f < t2) {
          row = g_rwp_constraint_padding;
          u = JD->drive;
          if (u & 4) row.rhs = i5 * rel8 - pr[2] * i9;
          if (u & 0x400) row.vel = dq[3] * i9 - dq[1] * pr[2] * cb;
          f = ca * rel8;
          c2[0] = c0[0] * f + c2[0] * i5;
          c2[1] = c0[1] * f + c2[1] * i5;
          c2[2] = c0[2] * f + c2[2] * i5;
          ANG(row, c2[0], c2[1], c2[2]);
          if ((u & 0x40000) == 0) {
            row.lo = -FMAX;
            row.hi = FMAX;
          } else {
            row.hi = JD->rot_force;
            row.lo = -JD->rot_force;
          }
          row.lo2 = row.hi2 = 0;
          f = dt * JD->rot_stiff + JD->rot_soft;
          if (f > 0) {
            f = 1.0f / f;
            CFM(row, f);
            row.tol = 0x3f4ccccd;
            row.erp = f * JD->rot_stiff * dt;
            rwp_constraint_store(s, &row);
            rows++;
          }
        }
      }
    }
  }

  /* twist limit */
  sa = JD->twist_hi * JD->twist_hi;
  sb = JD->twist_lo * JD->twist_lo;
  cd = 1.0f - sb;
  side = 0;
  if ((JD->dist_flags & 1) && (m = q[0] * sa * q[0] - (1.0f - sa) * q[3] * q[3], m > 0)) {
    side = 2;
    lim = sa;
  } else if ((JD->lo_flags & 1) && (m = sb * q[0] * q[0] - cd * q[3] * q[3], m > 0)) {
    side = 1;
    lim = sb;
  }
  if (side) {
    row = g_rwp_constraint_padding;
    row.rhs = m;
    tp0 = half[0] * TW_E;   /* the original keeps these two products as frame floats */
    tp1 = half[1] * TW_E;
    v[0] = half[3] * TW_X + tp0;
    v[1] = half[4] * TW_X + tp1;
    v[2] = TW_X * half[5] + TW_E * half[2];
    ANG(row, v[0], v[1], v[2]);
    row.lo = -FMAX;
    row.hi = 0;
    if (side == 1) {
      f = dt * JD->twist_hi_stiff + JD->twist_hi_soft;
      if (!(f > 0)) goto twist_done;
      f = 1.0f / f;
      CFM(row, f);
      row.lo2 = 0;
      row.erp = f * dt * JD->twist_hi_stiff;
    } else {
      row.lo2 = -FMAX;
    }
    row.hi2 = 0;
    row.tol = 0x3f4ccccd;
    rwp_constraint_store(s, &row);
    rows++;
  }
twist_done:

  /* swing limit (cone): the limits (cos) of both sides become the radii cA..cD of an ellipse; the row pushes back to its rim */
  sw1 = (JD->swing_lo_a < 0.999999f) ? JD->swing_lo_a : 0.999999f;
  ta = 0.999999f;
  if (JD->swing_lo_b < ta) ta = JD->swing_lo_b;
  sw2 = (JD->swing_hi_a < 0.999999f) ? JD->swing_hi_a : 0.999999f;
  tb = 0.999999f;
  if (JD->swing_hi_b < tb) tb = JD->swing_hi_b;
  r4 = sqrt(q[3] * q[3] + xx);
  r7 = sqrt(yy + zz);
  s1 = (1.0f + r4) * r4;
  s1 = s1 + s1;
  s1 = s1 * s1;
  i5 = 1.0f / (1.0f + r4);
  t1 = r7 * r4 + r7 * r4;
  ca = ((sw2 + 1.0f) * rel4) / (1.0f - sw2);
  cb = ((sw1 + 1.0f) * rel4) / (1.0f - sw1);
  cc = ((1.0f + tb) * rel8) / (1.0f - tb);
  cd = ((1.0f + ta) * rel8) / (1.0f - ta);
  m1 = ca * rel4 + cc * rel8;
  m2 = cb * rel4 + cd * rel8;
  if ((JD->dist_flags & 6) != 0 && m1 > s1) {
    inv = 1.0f / m1;
    side = 2;
    sq = sqrt(inv);
    e = i5 * r7 - t1 * sq;
    x = ca;
    y = cc;
  } else {
    if ((JD->lo_flags & 6) == 0 || !(m2 > s1)) goto swing_done;
    inv = 1.0f / m2;
    side = 1;
    sq = sqrt(inv);
    e = i5 * r7 - t1 * sq;
    x = cb;
    y = cd;
  }
  inv = inv * sq * t1;
  ka = inv * x;
  kb = inv * y;
  p1 = TN * mb[1];   /* the original keeps these two partial sums as frame floats */
  p2 = TN * mb[2];
  u1 = mb[5] * ka + p1;
  u2 = mb[6] * ka + p2;
  w0 = mb[8] * kb + mb[4] * ka + mb[0] * TN;
  w1 = mb[9] * kb + u1;
  w2 = mb[10] * kb + u2;
  c0[0] = ma[2] * w1 - ma[1] * w2;
  c0[1] = ma[0] * w2 - ma[2] * w0;
  c0[2] = ma[1] * w0 - ma[0] * w1;
  row = g_rwp_constraint_padding;
  row.rhs = -e;
  ANG(row, c0[0], c0[1], c0[2]);
  row.lo = 0;
  row.hi = FMAX;
  if (side == 1) {
    f = dt * JD->swing_stiff + JD->swing_soft;
    if (!(f > 0)) goto swing_done;
    f = 1.0f / f;
    CFM(row, f);
    row.hi2 = 0;
    row.erp = f * JD->swing_stiff * dt;
  } else {
    row.hi2 = FMAX;
  }
  row.lo2 = 0;
  row.tol = 0x3f4ccccd;
  rwp_constraint_store(s, &row);
  rows++;
swing_done:

  /* no row at all: a free row that does nothing, so the block is not empty */
  if (rows == 0) {
    row = g_rwp_constraint_padding;
    row.lo = -FMAX;
    row.hi = FMAX;
    row.lo2 = 0;
    row.hi2 = 0;
    row.tol = 0;
    rwp_constraint_store(s, &row);
  }
  rwp_constraints_0057f750(s);
}
