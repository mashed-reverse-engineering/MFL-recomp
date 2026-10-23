#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/* dot_product_3d_clamped */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "dot product of two 3D vectors clamped to [-1, 1]" */

/* Returns the dot product of the 3D vectors vec_a and vec_b (each three floats: x, y, z),
   clamped to the range [k_f_n1, float_1_0] (k_f_n1 = -1.0 (inferred), float_1_0 = 1.0).
   The result is computed in double (80-bit extended) precision. */
double dot_product_3d_clamped(float *vec_a,float *vec_b)

{
  double dot;

  /* sum of the x, y and z component products */
  dot = (double)vec_a[1] * (double)vec_b[1] + (double)*vec_a * (double)*vec_b;      /* summation order of the original: (y + x) + z */
  dot = dot + (double)vec_a[2] * (double)vec_b[2];
  /* clamp below to -1.0 */
  if (dot < (double)k_f_n1) {
    return (double)k_f_n1;
  }
  /* clamp above to 1.0 */
  if ((double)float_1_0 < dot) {
    dot = (double)float_1_0;
  }
  return dot;
}


