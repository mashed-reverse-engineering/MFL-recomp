/* rwp_solver_solve_groups (0x0057b9c0): run the row solver once per row group, sharing an iteration budget */
#include "mfl_tier2.h"
#include "rwp_common.h"

typedef struct MflQuad { float x, y, z, w; } MflQuad;

/* An array of float4 lanes passed around by value as {data, count, pad} (12 bytes). */
typedef struct MflQuadArray {
  MflQuad *v;
  unsigned int n;
  int pad;
} MflQuadArray;

typedef struct MflIntArray {
  int *v;
  unsigned int n;
} MflIntArray;

/* Solver state handed to the row solver: multipliers, residuals, iterations done (rwp_solver_lcp_iterate's MflLcpState). */
typedef struct MflLcpState {
  MflQuadArray lam;
  MflQuadArray acc;   /* 0xc */
  unsigned int iterations;   /* 0x18 */
} MflLcpState;

/* Per lane "off the lower / upper limit" flags (rwp_solver_lcp_iterate's MflLcpFree). */
typedef struct MflLcpFree {
  MflQuadArray off_lo;
  MflQuadArray off_hi;   /* 0xc */
} MflLcpFree;

/* The by-value tail of the row solver's argument list (67 words, after the two pointers). */
typedef struct MflLcpArgs {
  MflSolveByVal34 blk;
  MflQuadArray lo, hi, scale, acc0, lam0;   /* 0x88 */
  int unused[12];   /* 0xc4: never set by this caller */
  float bias;   /* 0xf4 */
  float weight;   /* 0xf8 */
  float tol;   /* 0xfc */
  int a67;   /* 0x100 */
  unsigned int max_iter;   /* 0x104 */
  int tail;   /* 0x108 */
} MflLcpArgs;

typedef void (*MflLcpSolveFn)(MflLcpState *s, MflLcpFree *fr, MflLcpArgs args);

/* The row groups of the batch solver (the part used here). */
typedef struct MflRowGroups {
  unsigned char pad00[0x64];
  MflQuadArray scale;   /* 0x64 */
  unsigned char pad70[0x60];
  unsigned int *weight;   /* 0xd0: share of the iteration budget per group */
  unsigned char padd4[8];
  unsigned int *rows;   /* 0xdc: rows per group (a multiple of 4) */
  unsigned char pade0[8];
  int *bodies;   /* 0xe8 */
  unsigned char padec[0x10];
  unsigned int count;   /* 0xfc */
  unsigned char pad100[4];
  unsigned int weight_total;   /* 0x104 */
  int share;   /* 0x108: split the budget by weight */
} MflRowGroups;

/* Calls `solve` (an MflLcpSolveFn) for every group of `groups`, each time on that group's slice of the arrays (lam_io / lo / hi / scale /
   acc0 / lam0 advance by the group's rows, the blk words 0 / 1 / 7 / 25 / 28 by its bodies and rows), and stores each group's
   iteration count in iters_out (n = number of groups).  Iteration limit per group: mode 0 = an even part of `budget` (at most 3 per
   group, the unused rest carried on, split by weight when groups->share is set), mode 1 = fixed_iters, else the previous group's.
   lam_io->n and acc_io->n grow by the rows / 4 of every group. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwp_solver_solve_groups(MflQuadArray *lam_io, MflQuadArray *acc_io, MflIntArray *iters_out, MflLcpFree *free_io,
                             MflSolveByVal34 blk, int a39, int a40, int a41, MflQuadArray lo, MflQuadArray hi, MflQuadArray acc0,
                             MflQuadArray lam0, MflRowGroups *groups, float rate, float bias, int a57, int a58, int mode,
                             unsigned int budget, unsigned int fixed_iters, float tol, int a63, int a64, int a65, int a66, int a67,
                             int tail, void *solve)
{
  MflLcpArgs args;
  MflLcpState st;
  MflLcpFree fr;
  unsigned int per, g, rows4, weight_left;
  int left, *bodies;

  args.blk = blk;
  args.weight = 1.0f / rate;
  weight_left = groups->weight_total;
  args.lo = lo;
  args.hi = hi;
  args.scale = groups->scale;
  args.acc0 = acc0;
  args.bias = bias;
  args.lam0 = lam0;
  args.tol = tol;
  args.a67 = a63;
  fr = *free_io;
  st.lam = *lam_io;
  st.acc = *acc_io;
  per = budget / groups->count;
  if (per >= 3)
    per = 3;
  left = budget - groups->count * per;
  if (groups->count == 0) {
    iters_out->n = groups->count;
    return;
  }
  g = 0;
  do {
    if (mode == 0) {
      left += per;
      if (!groups->share)
        args.max_iter = 0;
      else if (weight_left == 0)
        args.max_iter = left;
      else
        args.max_iter = groups->weight[g] * (left - per) / weight_left + per;
    } else if (mode == 1) {
      args.max_iter = fixed_iters;
    }
    bodies = (int *)args.blk.w[19];
    rows4 = groups->rows[g] >> 2;
    args.blk.w[31] = groups->bodies[g];
    args.lo.n = rows4;
    args.hi.n = rows4;
    args.scale.n = rows4;
    args.acc0.n = rows4;
    args.lam0.n = rows4;
    args.blk.w[33] = rows4;
    fr.off_lo.n = rows4;
    args.blk.w[2] = groups->bodies[g];
    fr.off_hi.n = groups->rows[g] >> 2;
    st.lam.n = groups->rows[g] >> 2;
    args.blk.w[32] = bodies[g];
    args.blk.w[8] = bodies[g];
    st.acc.n = groups->rows[g] >> 2;
    args.tail = tail;
    ((MflLcpSolveFn)solve)(&st, &fr, args);
    left -= st.iterations;
    if (left < 0)
      left = 0;
    weight_left -= groups->weight[g];
    args.blk.w[7] += bodies[g] * 4;
    args.blk.w[0] += bodies[g] * 96;
    rows4 = groups->rows[g] >> 2;
    args.blk.w[28] += rows4 * 8;
    args.blk.w[25] += rows4 * 8;
    args.blk.w[1] += groups->bodies[g] * 4;
    args.hi.v += rows4;
    args.lo.v += rows4;
    args.acc0.v += rows4;
    args.scale.v += rows4;
    st.lam.v += rows4;
    args.lam0.v += rows4;
    lam_io->n += groups->rows[g] >> 2;
    acc_io->n += groups->rows[g] >> 2;
    g++;
    iters_out->v[g - 1] = st.iterations;
  } while (g != groups->count);
  iters_out->n = groups->count;
}
