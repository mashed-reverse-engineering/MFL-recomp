#include "mfl_tier2.h"
/*005d3087*/ extern double _hypot();
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/* vec2_normalise_to_unit_square */

/* "out = (v / (2*length)) + 0.5 for the delta of two 2D points" */

void vec2_normalise_to_unit_square(float *out,float *from_pt,float *to_pt)

{
  float dx;
  float dy;
  double inv;

  dx = *to_pt - *from_pt;
  /* the assignment keeps the unrounded x87 difference as the second _hypot argument (fst + fstp qword in the original) */
  inv = (double)_hypot((double)dx,(double)(dy = to_pt[1] - from_pt[1]));
  inv = (double)float_1_0 / (inv + inv);
  *out = (float)((double)dx * inv + (double)k_f_0_5);
  out[1] = (float)(inv * (double)dy + (double)k_f_0_5);
  return;
}
