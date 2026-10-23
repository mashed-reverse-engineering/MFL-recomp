#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
/*005de1f8*/ extern float k_f_3_1415927;
extern double cos(double);
/* noise_interp_cos */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(a, b, t): cosine interpolation" */

/* Cosine interpolation from value_a (t = 0) to value_b (t = 1).
 * t is used as is, not clamped to 0..1.
 * The blend weight (1 - cos(t * pi)) / 2 eases in and out, so the
 * curve is flat at both ends. Returns the interpolated value. */
double __cdecl noise_interp_cos(float value_a,float value_b,float t)

{
  double weight;

  weight = (double)cos((double)t * (double)k_f_3_1415927);
  weight = ((double)(1.0) - weight) * (double)(0.5);
  return weight * (double)value_b + ((double)float_1_0 - weight) * (double)value_a;
}


