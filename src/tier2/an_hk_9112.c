#include "mfl_tier2.h"
/*005d3087*/ extern double _hypot();
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/* vec2_normalise_to_unit_square */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "out = (v / (2*length)) + 0.5 for the delta of two 2D points" */

void vec2_normalise_to_unit_square(float *param_1,float *param_2,float *param_3)

{
  float dx;
  float dy;
  double inv;

  dx = *param_3 - *param_2;
  /* the assignment keeps the unrounded x87 difference as the second _hypot argument (fst + fstp qword in the original) */
  inv = (double)_hypot((double)dx,(double)(dy = param_3[1] - param_2[1]));
  inv = (double)float_1_0 / (inv + inv);
  *param_1 = (float)((double)dx * inv + (double)k_f_0_5);
  param_1[1] = (float)(inv * (double)dy + (double)k_f_0_5);
  return;
}
