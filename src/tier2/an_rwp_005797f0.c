/* rwp_solver_prepare_rows (0x005797f0): solver stage: accumulate J^T * f per body and turn the Jacobian rows into M^-1 J^T */
#include "mfl_tier2.h"

typedef struct MflQuad { float v[4]; } MflQuad;

/* One Jacobian block: 6 rows of 4 floats (3 linear rows, then 3 angular rows; the 4th column is a lane of its own). */
typedef struct MflJacBlock { float r[24]; } MflJacBlock;

/* One body's mass data: a 3 x 3 inverse inertia (rows of 4 floats) with the inverse mass in the 4th float of the first row. */
typedef struct MflInvMass { float m[12]; } MflInvMass;

/* A body force: linear (3 + pad) and angular (3 + pad). */
typedef struct MflForce { float f[8]; } MflForce;

/* The solver stage of the solver function table (the caller stores its address, 571200).  The argument list is the solver's common one:
   a 14 word head, a 34 word block passed by value (counts, per-body arrays), then the per-step values; only the words used here are named.
   Body loop: for each body g (ngroups of them, nrows[g] rows each), every row k (counted over all bodies) adds J_k^T * force[g] to
   acc[index[k]] and is then scaled to M^-1 J^T: the 3 linear rows by the inverse mass, the 3 angular rows by the inverse inertia.
   Then for each of the n accumulators: acc -= (vel - a * b / h) / h with h = 1 / rate.
   Rounding: the sums run in the original's order, one operation per statement; partial sums the original keeps in an x87 register are
   doubles here, the ones it stores to its frame are the volatile float slots of `s` / `t` (cl would otherwise keep them unrounded on
   the x87 stack and reassociate the sums: denormal / overflow results differ). */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwp_solver_prepare_rows(MflQuad *acc, unsigned int n, int a3, MflJacBlock *rows, int a5, int a6, int a7, int a8, int a9, int a10,
                             int a11, int a12, int a13, int *index,
                             int a15, int a16, int a17, int a18, int a19, int *nrows, int a21, int a22, int a23, int a24, int a25,
                             int a26, int a27, int a28, int a29, int a30, int a31, int a32, int a33, int a34, unsigned int ngroups,
                             int a36, int a37, MflQuad *b, int a39, int a40, MflQuad *vel, int a42, int a43, MflQuad *a,
                             int a45, int a46, MflInvMass *invmass, int a48,
                             int a49, MflForce *force, int a51, int a52, float rate)
{
  float h = 1.0f / rate;
  unsigned int i, g, j, k, c;
  float *f, *m, *r, *out;
  volatile float s[4];   /* J^T f partial sums the original keeps in its frame */
  volatile float t[12];   /* the new angular rows (frame slots of the original) */
  double x0, x1, x2, x3;

  for (i = 0; i < n; i++) {
    acc[i].v[0] = 0;
    acc[i].v[1] = 0;
    acc[i].v[2] = 0;
    acc[i].v[3] = 0;
  }
  k = 0;
  for (g = 0; g < ngroups; g++) {
    for (j = 0; j < (unsigned int)nrows[g]; j++, k++) {
      f = force[g].f;
      m = invmass[g].m;
      r = rows[k].r;
      out = acc[index[j]].v;
      /* J^T f: lin rows times f[0..2], ang rows times f[4..6] */
      x0 = (double)f[0] * r[0];
      x1 = (double)r[1] * f[0];
      s[2] = f[0] * r[2];
      s[3] = f[0] * r[3];
      s[0] = (double)r[4] * f[1] + x0;
      x1 = (double)r[5] * f[1] + x1;
      x2 = (double)r[6] * f[1] + s[2];
      s[3] = r[7] * f[1] + s[3];
      s[0] = r[8] * f[2] + s[0];
      s[1] = (double)r[9] * f[2] + x1;
      x2 = (double)r[10] * f[2] + x2;
      x3 = (double)r[11] * f[2] + s[3];
      s[0] = r[12] * f[4] + s[0];
      s[1] = r[13] * f[4] + s[1];
      s[2] = (double)r[14] * f[4] + x2;
      x3 = (double)r[15] * f[4] + x3;
      x0 = (double)r[16] * f[5] + s[0];
      s[1] = r[17] * f[5] + s[1];
      s[2] = r[18] * f[5] + s[2];
      s[3] = (double)r[19] * f[5] + x3;
      x0 = (double)r[20] * f[6] + x0;
      x1 = (double)r[21] * f[6] + s[1];
      s[2] = r[22] * f[6] + s[2];
      s[3] = r[23] * f[6] + s[3];
      out[0] = x0 + out[0];
      out[1] = x1 + out[1];
      out[2] = s[2] + out[2];
      out[3] = s[3] + out[3];
      /* linear rows times the inverse mass */
      for (c = 0; c < 12; c++)
        r[c] = m[3] * r[c];
      /* angular rows: new row i = m[i] * row3 + m[4 + i] * row4 + m[8 + i] * row5 */
      x0 = (double)r[12] * m[0];
      x1 = (double)r[13] * m[0];
      t[2] = r[14] * m[0];
      t[3] = r[15] * m[0];
      t[0] = (double)m[4] * r[16] + x0;
      t[1] = (double)m[4] * r[17] + x1;
      t[2] = r[18] * m[4] + t[2];
      t[3] = r[19] * m[4] + t[3];
      x0 = (double)m[8] * r[20] + t[0];
      x1 = (double)m[8] * r[21] + t[1];
      t[2] = r[22] * m[8] + t[2];
      t[3] = r[23] * m[8] + t[3];

      x2 = (double)r[12] * m[1];
      x3 = (double)m[1] * r[13];
      t[6] = m[1] * r[14];
      t[7] = m[1] * r[15];
      t[4] = (double)m[5] * r[16] + x2;
      x3 = (double)m[5] * r[17] + x3;
      x2 = (double)r[18] * m[5] + t[6];
      t[7] = r[19] * m[5] + t[7];
      t[4] = m[9] * r[20] + t[4];
      t[5] = (double)m[9] * r[21] + x3;
      t[6] = (double)r[22] * m[9] + x2;
      t[7] = r[23] * m[9] + t[7];

      x2 = (double)r[12] * m[2];
      x3 = (double)r[13] * m[2];
      t[10] = r[14] * m[2];
      t[11] = r[15] * m[2];
      t[8] = (double)m[6] * r[16] + x2;
      x3 = (double)m[6] * r[17] + x3;
      x2 = (double)m[6] * r[18] + t[10];
      t[11] = r[19] * m[6] + t[11];
      t[8] = m[10] * r[20] + t[8];
      t[9] = (double)m[10] * r[21] + x3;
      t[10] = (double)m[10] * r[22] + x2;
      t[11] = r[23] * m[10] + t[11];
      for (c = 2; c < 12; c++)
        r[12 + c] = t[c];
      r[12] = x0;
      r[13] = x1;
    }
    index += nrows[g];
  }
  for (i = 0; i < n; i++) {
    acc[i].v[0] = acc[i].v[0] - (vel[i].v[0] - a[i].v[0] * b[i].v[0] * h) * h;
    acc[i].v[1] = acc[i].v[1] - (vel[i].v[1] - a[i].v[1] * b[i].v[1] * h) * h;
    acc[i].v[2] = acc[i].v[2] - (vel[i].v[2] - b[i].v[2] * (h * a[i].v[2])) * h;
    acc[i].v[3] = acc[i].v[3] - (vel[i].v[3] - a[i].v[3] * b[i].v[3] * h) * h;
  }
}
