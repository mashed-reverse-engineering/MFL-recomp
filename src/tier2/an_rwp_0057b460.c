#include "mfl_tier2.h"
#include "rwp_common.h"
#include <xmmintrin.h>

/* Both callees are called with their original argument words: 57b2f0 (arg-max scan, returns the sum of squares) and 57b190 (one clamped
   row update; the original passes one float more than the 7 that 57b190 reads, a trailing word). The views are {data, count, pad} words. */
extern float rwp_batch_0057b2f0(int *out_idx, int *out_lane, int a, int a_cnt, int a_pad, int v1_0, int v1_1, int v1_2,
                                int b, int b_cnt, int b_pad, int c, int c_cnt, int c_pad, float thr);
extern void rwp_batch_0057b190(int *col, int *row, int acc, int acc_cnt, int acc_pad, int lo_view, int lo_cnt, int lo_pad,
                               int hi_view, int hi_cnt, int hi_pad, int lam, int lam_cnt, int lam_pad, MflSolveByVal34 blk,
                               float lo, float lim_min, float lim_max, float hi, float scale, float bias, float weight, float tail);

/* rwp_batch_0057b460: the iteration driver of the batch solver for one group of four-lane rows.
   obj = {view1 (3 words), view2 (3 words), result at +0x18}, pp = {view3, view4}.  The first n float4 of `src_a` / `src_c` are copied into
   view2 / view1, and view3 / view4 get the not-equal masks of src_c against `ref_lo` / `ref_hi`.  Then the arg-max scan (57b2f0) picks
   a (idx, lane) pair and, up to `iters` times, 57b190 updates that row, the scan runs again, and a damped estimate
   (a, lo_sum, hi_sum) is kept; every `lim` steps a Newton-like re-estimate shortens `lim`.  *(obj + 0x18) = number of rows done. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
void rwp_batch_0057b460(int *obj, int *pp, MflSolveByVal34 blk,
                        float *ref_lo, int a9c, int aa0, float *ref_hi, int aa8, int aac, float *gb0, int ab4, int ab8,
                        __m128 *src_a, unsigned int n, int ac4, __m128 *src_c, int acc_, int ad0,
                        int ad4, int ad8, int adc, int ae0, int ae4, int ae8, int aec, int af0, int af4, int af8, int afc, int b00,
                        float bias_in, float weight_in, float thr, int b10, unsigned int iters, int adapt)
{
  int v1_data, v1_cnt, v1_pad, v2_data, v2_cnt, v2_pad, v3_data, v3_cnt, v3_pad, v4_data, v4_cnt, v4_pad;
  int idx, lane, off;
  unsigned int i, j, k, lim;
  float sum, a, b, l1, l2, prev, fcnt, g_lo, g_hi, g_min, g_max, g_scale, q, d, e, lf;
  __m128 x0, x1;

  v1_data = obj[0]; v1_cnt = obj[1]; v1_pad = obj[2];
  v2_data = obj[3]; v2_cnt = obj[4]; v2_pad = obj[5];
  v3_data = pp[0]; v3_cnt = pp[1]; v3_pad = pp[2];
  v4_data = pp[3]; v4_cnt = pp[4]; v4_pad = pp[5];
  for (i = 0; i < n; i++) {
    x0 = src_c[i];
    x1 = src_a[i];
    ((__m128 *)v2_data)[i] = x1;
    ((__m128 *)v1_data)[i] = x0;
    ((__m128 *)v3_data)[i] = _mm_cmpneq_ps(x0, ((__m128 *)ref_lo)[i]);
    ((__m128 *)v4_data)[i] = _mm_cmpneq_ps(x0, ((__m128 *)ref_hi)[i]);
  }
  sum = rwp_batch_0057b2f0(&idx, &lane, v2_data, v2_cnt, v2_pad, v1_data, v1_cnt, v1_pad, v3_data, v3_cnt, v3_pad, v4_data, v4_cnt, v4_pad, thr);
  prev = sum;
  off = lane + idx * 4;
  g_lo = ((float *)v2_data)[off];
  g_hi = ((float *)v1_data)[off];
  g_min = ref_lo[off];
  g_max = ref_hi[off];
  g_scale = gb0[off];
  a = 0.0f;
  l1 = 0.0f;
  l2 = sum;
  k = 0;
  lim = v2_cnt + 1;
  fcnt = (float)(unsigned int)v2_cnt;
  if (iters == 0) {
    obj[6] = 0;
    return;
  }
  j = 0;
  do {
    if (idx == -1) {
      obj[6] = j;
      return;
    }
    rwp_batch_0057b190(&idx, &lane, v2_data, v2_cnt, v2_pad, v3_data, v3_cnt, v3_pad, v4_data, v4_cnt, v4_pad, v1_data, v1_cnt, v1_pad,
                       blk, g_lo, g_min, g_max, g_hi, g_scale, bias_in, weight_in, thr);
    b = rwp_batch_0057b2f0(&idx, &lane, v2_data, v2_cnt, v2_pad, v1_data, v1_cnt, v1_pad, v3_data, v3_cnt, v3_pad, v4_data, v4_cnt, v4_pad, thr);
    off = lane + idx * 4;
    g_lo = ((float *)v2_data)[off];
    g_hi = ((float *)v1_data)[off];
    g_min = ref_lo[off];
    g_max = ref_hi[off];
    g_scale = gb0[off];
    if (adapt != 0) {
      k++;
      if (k > lim) k = 0;
      if (k == 0) {
        if (!(a >= prev)) {
          prev = a;
          a = (a + a) * 0.5f;
          q = 1.0f / a;
          d = l1 - l2;
          e = q * d * 100.0f + 1.0f;
          if ((double)lim > e) {
            lf = q * d * fcnt;
            lim = _mm_cvtss_si32(_mm_load_ss(&lf)) + 1;
          }
          l1 = b;
          l2 = b;
        } else {
          idx = -1;
        }
      }
      a = (b - a) * 0.5f + a;
      if (b > l1) l1 = b;
      if (b < l2) l2 = b;
    }
    j++;
  } while (j < iters);
  obj[6] = j;
}
