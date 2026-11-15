#include "mfl_tier2.h"
#include <xmmintrin.h>

/* Row preparation of the batched solver (SSE, four constraints per float4 lane).  `rows` holds six float4 per row slot (96 bytes: linear x y z and
   angular x y z Jacobian columns of the four lanes); group k has cnts[k] consecutive row slots and the row slot j of the groups' running list
   adds into acc[idx[j]].  ab[2k] / ab[2k + 1] are the linear / angular velocities of the group's body (float4 each, x y z splatted over the lanes),
   cde[3k] .. cde[3k + 2] the body's inverse mass (w of the first) and symmetric 3x3 inverse inertia (rows x y z / y z / z).
   Per row: acc += J . v (the six columns weighted with the splatted velocity), then the row is replaced by J M^-1: linear columns scaled by the
   inverse mass, angular columns multiplied by the inverse inertia.  Finally the right hand side is corrected:
   acc -= (vel_arr - a_arr * b_arr * (1 / dt)) * (1 / dt) for all `count` lanes.  The parameters between a2 and dt are a by-value solver block (flat words, only
   the members used here are named).  `acc` is cleared first. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
void rwp_batch_0057ab00(
  __m128 *acc, int count, int a2, float *rows, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12,
  int *idx, int a14, int a15, int a16, int a17, int a18, int *cnts, int a20, int a21, int a22, int a23, int a24, int a25,
  int a26, int a27, int a28, int a29, int a30, int a31, int a32, int a33, int n, int a35, int a36, __m128 *b_arr, int a38,
  int a39, __m128 *vel_arr, int a41, int a42, __m128 *a_arr, int a44, int a45, __m128 *cde, int a47, int a48, __m128 *ab,
  int a50, int a51, float dt)
{
  int i, j, k, c, t, ix;
  float *r;
  float inv;
  __m128 ax, ay, az, bx, by, bz, cx, cy, cz, cw, dy, dz, ez;
  __m128 jlin_x, jlin_y, jlin_z, jang_x, jang_y, jang_z, sum, s;
  __m128 *p;

  inv = 1.0f / dt;
  for (i = 0; i != count; i++) acc[i] = _mm_setzero_ps();
  t = 0;
  for (k = 0; k != n; k++) {
    c = cnts[k];
    if (c != 0) {
      p = ab + 2 * k;
      ax = _mm_shuffle_ps(p[0], p[0], 0x00);
      ay = _mm_shuffle_ps(p[0], p[0], 0x55);
      az = _mm_shuffle_ps(p[0], p[0], 0xaa);
      bx = _mm_shuffle_ps(p[1], p[1], 0x00);
      by = _mm_shuffle_ps(p[1], p[1], 0x55);
      bz = _mm_shuffle_ps(p[1], p[1], 0xaa);
      p = cde + 3 * k;
      cw = _mm_shuffle_ps(p[0], p[0], 0xff);
      cx = _mm_shuffle_ps(p[0], p[0], 0x00);
      cy = _mm_shuffle_ps(p[0], p[0], 0x55);
      cz = _mm_shuffle_ps(p[0], p[0], 0xaa);
      dy = _mm_shuffle_ps(p[1], p[1], 0x55);
      dz = _mm_shuffle_ps(p[1], p[1], 0xaa);
      ez = _mm_shuffle_ps(p[2], p[2], 0xaa);
      for (j = 0; j != c; j++) {
        r = rows + t * 24;
        jlin_x = _mm_load_ps(r);
        jlin_y = _mm_load_ps(r + 4);
        jlin_z = _mm_load_ps(r + 8);
        jang_x = _mm_load_ps(r + 12);
        jang_y = _mm_load_ps(r + 16);
        jang_z = _mm_load_ps(r + 20);
        ix = idx[j];
        sum = _mm_mul_ps(jlin_x, ax);
        sum = _mm_add_ps(sum, _mm_mul_ps(jlin_y, ay));
        sum = _mm_add_ps(sum, _mm_mul_ps(jlin_z, az));
        sum = _mm_add_ps(sum, _mm_mul_ps(jang_x, bx));
        sum = _mm_add_ps(sum, _mm_mul_ps(jang_y, by));
        sum = _mm_add_ps(sum, _mm_mul_ps(jang_z, bz));
        sum = _mm_add_ps(sum, acc[ix]);
        acc[ix] = sum;
        _mm_store_ps(r, _mm_mul_ps(jlin_x, cw));
        _mm_store_ps(r + 4, _mm_mul_ps(jlin_y, cw));
        _mm_store_ps(r + 8, _mm_mul_ps(jlin_z, cw));
        _mm_store_ps(r + 12, _mm_add_ps(_mm_add_ps(_mm_mul_ps(jang_x, cx), _mm_mul_ps(jang_y, cy)), _mm_mul_ps(jang_z, cz)));
        _mm_store_ps(r + 16, _mm_add_ps(_mm_add_ps(_mm_mul_ps(jang_x, cy), _mm_mul_ps(jang_y, dy)), _mm_mul_ps(jang_z, dz)));
        _mm_store_ps(r + 20, _mm_add_ps(_mm_add_ps(_mm_mul_ps(jang_x, cz), _mm_mul_ps(jang_y, dz)), _mm_mul_ps(jang_z, ez)));
        t++;
      }
    }
    idx += c;
  }
  s = _mm_load1_ps(&inv);
  for (i = 0; i != count; i++)
    acc[i] = _mm_sub_ps(acc[i], _mm_mul_ps(_mm_sub_ps(vel_arr[i], _mm_mul_ps(_mm_mul_ps(a_arr[i], b_arr[i]), s)), s));
}
