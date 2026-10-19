#include "mfl_tier2.h"
/*004b38f7*/ extern int finite();
/*005df83c*/ extern float k_f_1_0en6;
/* vec3_is_valid_small */

/* Returns 1 if the 3-float vector `vec` is valid: all three components are finite (not NaN/inf)
 * and its squared length is greater than 1e-6, i.e. it is not "small" (near zero). Returns 0 otherwise.
 * The old comment said "squared length below 1e-6"; the comparison below does the opposite, so this follows the code.
 * (the intent, reject near-zero vectors, is inferred from the name) */

undefined4 __cdecl vec3_is_valid_small(float *vec)

{
  float length_sq;
  int is_finite;

  /* every component must be finite */
  is_finite = finite((double)*vec);
  if (is_finite != 0) {
    is_finite = finite((double)vec[1]);
    if (is_finite != 0) {
      is_finite = finite((double)vec[2]);
      /* squared length must be > 1e-6: (length_sq < k) == (length_sq == k) is true for
         length_sq > k and for NaN (unordered), false for length_sq <= k */
      if ((is_finite != 0) &&
         (length_sq = vec[2] * vec[2] + *vec * *vec + vec[1] * vec[1],
         length_sq < k_f_1_0en6 == (length_sq == k_f_1_0en6))) {
        return 1;
      }
    }
  }
  return 0;
}


