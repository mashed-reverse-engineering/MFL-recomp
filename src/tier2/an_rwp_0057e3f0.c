#include "mfl_tier2.h"
#include <xmmintrin.h>

/* Contact pair step of the batch solver.  The records are 0x100 bytes: four blocks of {x0, y0, x1, y1} float4 rows (the w lane
   of x0 / x1 holds the body index of side 0 / 1, -1 = static).  The bodies are 48 bytes: three float4 rows r0 r1 r2 (the 3x3
   matrix; r0.w = the body's scale).  Per record and per side the four blocks give the lane sums
   |R*y|^2 + |scale * x|^2 (xyz only); both sides are added, the per-element value `bias[i] * s` is added, and
   the result (D) gets a refined reciprocal square root (rsqrtps + one Newton step, 0 for the lanes where D == 0).
   The xyz parts of the 16 rows of the record are scaled with that factor lane by lane (4x4 transposes), and the six
   float4 arrays v18 v24 v30 v3c v54 v60 / v48 are updated element by element (a sentinel +-FLT_MAX keeps the scale 1). */
static const __declspec(align(16)) unsigned int xyz_bits[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0 };

/* one block: Q = (r0^2, r1^2, r2^2, |u|^2) with rj = dot(Rj, y), u = x * s (xyz) */
#define BLOCK(X, Y, Q) { \
    __m128 u, y, w0, w1, w2, uu, t, ta, tb, tc, r, rr; \
    u = _mm_mul_ps(_mm_and_ps(msk, (X)), s4); \
    y = _mm_and_ps(msk, (Y)); \
    w1 = _mm_mul_ps(_mm_and_ps(msk, r1), y); \
    w0 = _mm_mul_ps(_mm_and_ps(msk, r0), y); \
    w2 = _mm_mul_ps(_mm_and_ps(msk, r2), y); \
    uu = _mm_mul_ps(u, u); \
    t = _mm_unpacklo_ps(w2, uu); ta = _mm_unpackhi_ps(w2, uu); \
    tb = _mm_unpackhi_ps(w0, w1); tc = _mm_unpacklo_ps(w0, w1); \
    r = _mm_add_ps(_mm_movelh_ps(tb, ta), _mm_movelh_ps(tc, t)); \
    r = _mm_add_ps(r, _mm_movehl_ps(t, tc)); \
    rr = _mm_mul_ps(r, r); \
    (Q) = _mm_shuffle_ps(rr, _mm_movehl_ps(r, rr), 0xc4); }

/* one side of the record: the four blocks reduced to one float4 (lane j = sum of the block j) */
#define SIDE(X, Y, OUT) { \
    __m128 qa, qb, qc, qd, h1, h2; \
    BLOCK(rec[0].X, rec[0].Y, qa) BLOCK(rec[1].X, rec[1].Y, qb) BLOCK(rec[2].X, rec[2].Y, qc) BLOCK(rec[3].X, rec[3].Y, qd) \
    h1 = _mm_add_ps(_mm_movelh_ps(qa, qb), _mm_movehl_ps(qb, qa)); \
    h2 = _mm_add_ps(_mm_movelh_ps(qc, qd), _mm_movehl_ps(qd, qc)); \
    (OUT) = _mm_add_ps(_mm_shuffle_ps(h1, h2, 0xdd), _mm_shuffle_ps(h1, h2, 0x88)); }

/* rows A B C D of the four blocks: xyz of row j times lane j of R (transpose, scale the first three, transpose back) */
#define SCALE4(F) { \
    __m128 a, b, c, d, lo_ab, hi_ab, lo_cd, hi_cd, t0, t1, t2, t3, s01lo, s01hi, s23lo, s23hi; \
    a = rec[0].F; b = rec[1].F; c = rec[2].F; d = rec[3].F; \
    lo_ab = _mm_shuffle_ps(a, b, 0x44); hi_ab = _mm_shuffle_ps(a, b, 0xee); \
    lo_cd = _mm_shuffle_ps(c, d, 0x44); hi_cd = _mm_shuffle_ps(c, d, 0xee); \
    t0 = _mm_mul_ps(_mm_shuffle_ps(lo_ab, lo_cd, 0x88), fac); \
    t3 = _mm_shuffle_ps(hi_ab, hi_cd, 0xdd); \
    t1 = _mm_mul_ps(_mm_shuffle_ps(lo_ab, lo_cd, 0xdd), fac); \
    t2 = _mm_mul_ps(_mm_shuffle_ps(hi_ab, hi_cd, 0x88), fac); \
    s01lo = _mm_shuffle_ps(t0, t1, 0x44); s01hi = _mm_shuffle_ps(t0, t1, 0xee); \
    s23lo = _mm_shuffle_ps(t2, t3, 0x44); s23hi = _mm_shuffle_ps(t2, t3, 0xee); \
    rec[0].F = _mm_shuffle_ps(s01lo, s23lo, 0x88); rec[1].F = _mm_shuffle_ps(s01lo, s23lo, 0xdd); \
    rec[2].F = _mm_shuffle_ps(s01hi, s23hi, 0x88); rec[3].F = _mm_shuffle_ps(s01hi, s23hi, 0xdd); }

/* *P *= (P != SENT) ? sq : 1 */
#define SENTINEL_MUL(P, SENT) { \
    __m128 x, m; \
    x = _mm_load_ps((float *)&(P)); \
    m = _mm_cmpneq_ps(x, (SENT)); \
    _mm_store_ps((float *)&(P), _mm_mul_ps(x, _mm_or_ps(_mm_and_ps(m, sq), _mm_andnot_ps(m, one)))); }

typedef struct MflPairRec { __m128 x0, y0, x1, y1; } MflPairRec;      /* 64 bytes; a record is 4 of them */
typedef struct MflBody48 { __m128 r0, r1, r2; } MflBody48;

/* TIER2: unvalidated 2026-11-02 authority=diffemu */
void rwp_batch_0057e3f0(MflPairRec *rec, int a3, int a4, int a5, __m128 *v18, int a7, int a8, __m128 *v24, int a10, int a11,
                        __m128 *v30, int a13, int a14, __m128 *v3c, int a16, int a17, __m128 *v48, int a19, int a20,
                        __m128 *v54, int a22, int a23, __m128 *v60, int n, int a26, MflBody48 *bodies, int a28, int a29,
                        float scale)
{
  __m128 msk, zero, one, three, half, flt_max, neg_flt_max, s4, acc, f0, f1, d, m, rs, fac, sq, b, r0, r1, r2;
  int i0, i1, i;
  const MflBody48 *p;

  zero = _mm_setzero_ps();
  s4 = _mm_set_ps1(scale);
  msk = _mm_load_ps((const float *)xyz_bits);
  one = _mm_set_ps1(1.0f);
  three = _mm_set_ps1(3.0f);
  half = _mm_set_ps1(0.5f);
  flt_max = _mm_set_ps1(3.4028235e38f);
  neg_flt_max = _mm_set_ps1(-3.4028235e38f);
  if (n == 0) return;
  i = 0;
  do {
    i0 = ((int *)&rec[0].x0)[3];
    i1 = ((int *)&rec[0].x1)[3];
    acc = zero;
    sq = zero;
    if (i0 != -1) {
      p = &bodies[i0];
      r0 = p->r0; r1 = p->r1; r2 = p->r2;
      s4 = _mm_set_ps1(((float *)p)[3]);
      SIDE(x0, y0, f0)
      acc = f0;
    }
    if (i1 != -1) {
      p = &bodies[i1];
      r0 = p->r0; r1 = p->r1; r2 = p->r2;
      s4 = _mm_set_ps1(((float *)p)[3]);
      SIDE(x1, y1, f1)
      acc = _mm_add_ps(acc, f1);
    }
    b = _mm_load_ps((float *)&v48[i]);
    acc = _mm_add_ps(acc, _mm_mul_ps(_mm_set_ps1(scale), b));
    d = acc;
    m = _mm_cmpneq_ps(d, zero);
    fac = d;
    if (_mm_movemask_ps(m)) {
      rs = _mm_rsqrt_ps(d);
      fac = _mm_sub_ps(three, _mm_mul_ps(_mm_mul_ps(d, rs), rs));
      fac = _mm_and_ps(m, _mm_mul_ps(half, _mm_mul_ps(rs, fac)));
      sq = _mm_mul_ps(d, fac);
    }
    SCALE4(x0) SCALE4(y0) SCALE4(x1) SCALE4(y1)
    SENTINEL_MUL(v18[i], neg_flt_max)
    SENTINEL_MUL(v24[i], flt_max)
    SENTINEL_MUL(v30[i], neg_flt_max)
    SENTINEL_MUL(v3c[i], flt_max)
    _mm_store_ps((float *)&v54[i], _mm_mul_ps(fac, _mm_load_ps((float *)&v54[i])));
    _mm_store_ps((float *)&v60[i], _mm_mul_ps(fac, _mm_load_ps((float *)&v60[i])));
    _mm_store_ps((float *)&v48[i], _mm_mul_ps(b, _mm_mul_ps(fac, fac)));
    rec += 4;
    i++;
  } while (--n);
}
