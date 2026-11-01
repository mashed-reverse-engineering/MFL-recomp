#include "mfl_tier2.h"
#include <xmmintrin.h>

/* Solver step: scatter one constraint lane's impulse back into the velocity accumulators.  Constraints are stored four to a
   group (idx >> 2 = group, idx & 3 = lane); `jac` holds six float4 rows per row slot (96 bytes: the linear x y z and angular x y z
   Jacobian columns of the four lanes).  For both bodies of the pair (pairs[2 * group]; -1 = static) the lane of the row slot
   is scaled by `delta`, then for each of the body's rows (row_first[body] .. + row_count[body]) the row slot's six float4 are
   weighted with the splatted linear / angular vector and added to the accumulator vector acc[acc_index[row]].
   The parameters between `a5` and `delta` are a 34 dword MflSolveByVal34 passed by value (see 57b190); only the members
   named here are used: jac (w0), row_first (w4), acc_index (w7), row_count (w16), pairs (w25), slots (w28). */
/* TIER2: unvalidated 2026-11-01 authority=diffemu */
void rwp_batch_0057ae50(__m128 *acc, int a4, int a5,
                        float *jac, int w1, int w2, int w3, int *row_first, int w5, int w6, int *acc_index,
                        int w8, int w9, int w10, int w11, int w12, int w13, int w14, int w15,
                        int *row_count, int w17, int w18, int w19, int w20, int w21, int w22, int w23, int w24,
                        int *pairs, int w26, int w27, int *slots, int w29, int w30, int w31, int w32, int w33,
                        float delta, int idx)
{
  __declspec(align(16)) float lin0[4], ang0[4], lin1[4], ang1[4];
  int group, lane, body0, body1, slot, row, n, k;
  float *r;
  __m128 lx, ly, lz, ax, ay, az, sum;

  group = idx >> 2;
  lane = idx & 3;
  body0 = pairs[group * 2];
  body1 = pairs[group * 2 + 1];
  if (body0 != -1) {
    r = jac + slots[group * 2] * 24 + lane;
    lin0[0] = delta * r[0];
    lin0[1] = delta * r[4];
    lin0[2] = delta * r[8];
    ang0[0] = delta * r[12];
    ang0[1] = delta * r[16];
    ang0[2] = delta * r[20];
  }
  if (body1 != -1) {
    r = jac + slots[(idx / 4) * 2 + 1] * 24 + lane;
    lin1[0] = delta * r[0];
    lin1[1] = delta * r[4];
    lin1[2] = delta * r[8];
    ang1[0] = delta * r[12];
    ang1[1] = delta * r[16];
    ang1[2] = delta * r[20];
  }
  if (body0 != -1 && row_count[body0] != 0) {
    lx = _mm_shuffle_ps(_mm_load_ps(lin0), _mm_load_ps(lin0), 0x00);
    ly = _mm_shuffle_ps(_mm_load_ps(lin0), _mm_load_ps(lin0), 0x55);
    lz = _mm_shuffle_ps(_mm_load_ps(lin0), _mm_load_ps(lin0), 0xaa);
    ax = _mm_shuffle_ps(_mm_load_ps(ang0), _mm_load_ps(ang0), 0x00);
    ay = _mm_shuffle_ps(_mm_load_ps(ang0), _mm_load_ps(ang0), 0x55);
    az = _mm_shuffle_ps(_mm_load_ps(ang0), _mm_load_ps(ang0), 0xaa);
    n = 0;
    do {
      row = row_first[body0] + n;
      r = jac + row * 24;
      sum = _mm_add_ps(acc[acc_index[row]], _mm_mul_ps(_mm_load_ps(r), lx));
      sum = _mm_add_ps(sum, _mm_mul_ps(_mm_load_ps(r + 4), ly));
      sum = _mm_add_ps(sum, _mm_mul_ps(_mm_load_ps(r + 8), lz));
      sum = _mm_add_ps(sum, _mm_mul_ps(_mm_load_ps(r + 12), ax));
      sum = _mm_add_ps(sum, _mm_mul_ps(_mm_load_ps(r + 16), ay));
      sum = _mm_add_ps(sum, _mm_mul_ps(_mm_load_ps(r + 20), az));
      _mm_store_ps((float *)&acc[acc_index[row]], sum);
      n++;
    } while (n != row_count[body0]);
  }
  if (body1 != -1 && row_count[body1] != 0) {
    lx = _mm_shuffle_ps(_mm_load_ps(lin1), _mm_load_ps(lin1), 0x00);
    ly = _mm_shuffle_ps(_mm_load_ps(lin1), _mm_load_ps(lin1), 0x55);
    lz = _mm_shuffle_ps(_mm_load_ps(lin1), _mm_load_ps(lin1), 0xaa);
    ax = _mm_shuffle_ps(_mm_load_ps(ang1), _mm_load_ps(ang1), 0x00);
    ay = _mm_shuffle_ps(_mm_load_ps(ang1), _mm_load_ps(ang1), 0x55);
    az = _mm_shuffle_ps(_mm_load_ps(ang1), _mm_load_ps(ang1), 0xaa);
    n = 0;
    do {
      row = row_first[body1] + n;
      r = jac + row * 24;
      sum = _mm_add_ps(acc[acc_index[row]], _mm_mul_ps(_mm_load_ps(r), lx));
      sum = _mm_add_ps(sum, _mm_mul_ps(_mm_load_ps(r + 4), ly));
      sum = _mm_add_ps(sum, _mm_mul_ps(_mm_load_ps(r + 8), lz));
      sum = _mm_add_ps(sum, _mm_mul_ps(_mm_load_ps(r + 12), ax));
      sum = _mm_add_ps(sum, _mm_mul_ps(_mm_load_ps(r + 16), ay));
      sum = _mm_add_ps(sum, _mm_mul_ps(_mm_load_ps(r + 20), az));
      _mm_store_ps((float *)&acc[acc_index[row]], sum);
      n++;
    } while (n != row_count[body1]);
  }
}
