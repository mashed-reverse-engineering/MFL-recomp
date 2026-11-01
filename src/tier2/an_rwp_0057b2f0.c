#include "mfl_tier2.h"
#include <xmmintrin.h>

/* The arguments after the two outputs are four 12 byte views {data, count, pad} passed by value (the second is unused). */
/* Four-lane running arg-max over `count` float4 elements.  Per lane the candidate is |a| when either
   (-(a & c)) or (a & b) exceeds the lane's current best (thr to start with); the winning lane and its
   element index are written to *out_idx / *out_lane (-1 / -1 when no lane beat thr).  Returns the sum
   of squares of all the floats of `a`. */
/* TIER2: unvalidated 2026-11-01 authority=diffemu */
float rwp_batch_0057b2f0(int *out_idx, int *out_lane,
                         __m128 *a, int count, int a_pad,
                         int v1_0, int v1_1, int v1_2,
                         __m128 *b, int b_count, int b_pad,
                         __m128 *c, int c_count, int c_pad, float thr)
{
  static const __declspec(align(16)) unsigned int sign_bits[4] = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
  __m128 sign, best, idx, step, bidx;
  __m128 x, y, mask;
  float best_v[4], idx_v[4];
  float sum;
  float *p;
  float cur;
  int i, j, lane, result;

  sign = _mm_load_ps((const float *)sign_bits);
  best = _mm_set_ps1(thr);
  idx = _mm_setzero_ps();
  step = _mm_set_ps1(1.0f);
  result = -1;
  lane = -1;
  bidx = _mm_setzero_ps();
  for (i = 0; i < count; i++) {
    x = _mm_xor_ps(sign, _mm_and_ps(a[i], c[i]));
    y = _mm_and_ps(a[i], b[i]);
    mask = _mm_or_ps(_mm_cmplt_ps(best, x), _mm_cmplt_ps(best, y));
    if (_mm_movemask_ps(mask)) {
      best = _mm_or_ps(_mm_and_ps(mask, _mm_andnot_ps(sign, a[i])), _mm_andnot_ps(mask, best));
      bidx = _mm_or_ps(_mm_and_ps(mask, idx), _mm_andnot_ps(mask, bidx));
    }
    idx = _mm_add_ps(idx, step);
  }
  _mm_store_ps(best_v, best);
  _mm_store_ps(idx_v, bidx);
  sum = 0.0f;
  p = (float *)a;
  for (i = 0; i < count; i++)
    for (j = 0; j < 4; j++, p++)
      sum += *p * *p;
  for (j = 0; j < 4; j++) {
    cur = best_v[j];
    if (cur > thr) {
      thr = cur;
      lane = j;
    }
  }
  if (lane != -1)
    result = _mm_cvtss_si32(_mm_load_ss(&idx_v[lane]));
  *out_idx = result;
  *out_lane = lane;
  return sum;
}
