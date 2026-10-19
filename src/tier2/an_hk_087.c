#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/* dot_product_3d_clamped */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "dot product of two 3D vectors clamped to [-1, 1]" */

/* Returns the dot product of the 3D vectors vec_a and vec_b (each three floats: x, y, z),
   clamped to the range [k_f_n1, float_1_0] (k_f_n1 = -1.0 (inferred), float_1_0 = 1.0).
   The result is computed in float10 (80-bit extended) precision. */
float10 dot_product_3d_clamped(float *vec_a,float *vec_b)

{
  float10 dot;

  /* sum of the x, y and z component products */
  dot = (float10)vec_a[2] * (float10)vec_b[2] +
          (float10)*vec_a * (float10)*vec_b + (float10)vec_a[1] * (float10)vec_b[1];
  /* clamp below to -1.0 */
  if (dot < (float10)k_f_n1) {
    return (float10)k_f_n1;
  }
  /* clamp above to 1.0 */
  if ((float10)float_1_0 < dot) {
    dot = (float10)float_1_0;
  }
  return dot;
}


