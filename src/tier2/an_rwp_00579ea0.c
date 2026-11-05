/* rwp_solver_lcp_iterate (0x00579ea0): solver stage: projected Gauss-Seidel iterations over the constraint rows */
#include "mfl_tier2.h"
#include "rwp_common.h"

typedef struct MflQuad { float x, y, z, w; } MflQuad;

/* An array of float4 lanes passed around by value as {data, count, pad} (12 bytes). */
typedef struct MflQuadArray {
  MflQuad *v;
  unsigned int n;
  int pad;
} MflQuadArray;

/* Solver state: the multipliers (lambda), the accumulated row residuals and the iteration count of the last solve. */
typedef struct MflLcpState {
  MflQuadArray lam;
  MflQuadArray acc;   /* 0xc */
  unsigned int iterations;   /* 0x18 */
} MflLcpState;

/* Per lane 1.0 while the multiplier is off its lower / upper limit, 0.0 on it. */
typedef struct MflLcpFree {
  MflQuadArray off_lo;
  MflQuadArray off_hi;   /* 0xc */
} MflLcpFree;

/*009488e0*/ extern unsigned int g_rwp_solver_iteration;

extern void rwp_solver_argmax_lanes(int *col, int *lane, MflQuadArray acc, MflQuadArray lam, MflQuadArray lo, MflQuadArray hi,
                                    MflQuadArray off_lo, MflQuadArray off_hi, float tol);
extern void rwp_solver_row_update_clamped(int *col, int *lane, MflQuadArray acc, MflQuadArray off_lo, MflQuadArray off_hi,
                                          MflQuadArray lam, MflSolveByVal34 blk, MflQuadArray lo, MflQuadArray hi,
                                          float acc_v, float lo_v, float hi_v, float lam_v, float scale_v, float bias, float weight,
                                          float tol);

#define LANE(a, i) (((float *)(a).v)[i])

/* Starts from acc0 / lam0 (n0 = acc0.n lanes copied), marks per lane whether lambda sits on its limits, then repeatedly picks the
   worst row (rwp_solver_argmax_lanes, tolerance tol) and updates it (rwp_solver_row_update_clamped), at most max_iter times or until
   no row is left.  The iteration count goes to g_rwp_solver_iteration and s->iterations.  blk: the solver's common by-value block,
   passed through to the row update. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwp_solver_lcp_iterate(MflLcpState *s, MflLcpFree *fr, MflSolveByVal34 blk, MflQuadArray lo, MflQuadArray hi,
                            MflQuadArray scale, MflQuadArray acc0, MflQuadArray lam0, int a52, int a53, int a54, int a55, int a56,
                            int a57, int a58, int a59, int a60, int a61, int a62, int a63, float bias, float weight, float tol,
                            int a67, unsigned int max_iter)
{
  MflQuadArray lam = s->lam, acc = s->acc;
  MflQuadArray off_lo = fr->off_lo, off_hi = fr->off_hi;
  unsigned int i, idx;
  int col, lane;
  float acc_v, lam_v, lo_v, hi_v, scale_v;

  for (i = 0; i < acc0.n; i++) {
    acc.v[i] = acc0.v[i];
    lam.v[i] = lam0.v[i];
  }
  for (i = 0; i < lam.n; i++) {
    off_lo.v[i].x = lam.v[i].x == lo.v[i].x ? 0.0f : 1.0f;
    off_lo.v[i].y = lam.v[i].y == lo.v[i].y ? 0.0f : 1.0f;
    off_lo.v[i].z = lam.v[i].z == lo.v[i].z ? 0.0f : 1.0f;
    off_lo.v[i].w = lam.v[i].w == lo.v[i].w ? 0.0f : 1.0f;
    off_hi.v[i].x = lam.v[i].x == hi.v[i].x ? 0.0f : 1.0f;
    off_hi.v[i].y = lam.v[i].y == hi.v[i].y ? 0.0f : 1.0f;
    off_hi.v[i].z = lam.v[i].z == hi.v[i].z ? 0.0f : 1.0f;
    off_hi.v[i].w = lam.v[i].w == hi.v[i].w ? 0.0f : 1.0f;
  }
  rwp_solver_argmax_lanes(&col, &lane, acc, lam, lo, hi, off_lo, off_hi, tol);
  idx = col * 4 + lane;
  acc_v = LANE(acc, idx);
  lam_v = LANE(lam, idx);
  lo_v = LANE(lo, idx);
  hi_v = LANE(hi, idx);
  scale_v = LANE(scale, idx);
  g_rwp_solver_iteration = 0;
  if (max_iter == 0) {
    s->iterations = 0;
    return;
  }
  while (col != -1) {
    rwp_solver_row_update_clamped(&col, &lane, acc, off_lo, off_hi, lam, blk, lo, hi, acc_v, lo_v, hi_v, lam_v, scale_v, bias, weight,
                                  tol);
    rwp_solver_argmax_lanes(&col, &lane, acc, lam, lo, hi, off_lo, off_hi, tol);
    idx = col * 4 + lane;
    acc_v = LANE(acc, idx);
    lam_v = LANE(lam, idx);
    lo_v = LANE(lo, idx);
    hi_v = LANE(hi, idx);
    scale_v = LANE(scale, idx);
    if (++g_rwp_solver_iteration >= max_iter)
      break;
  }
  s->iterations = g_rwp_solver_iteration;
}
