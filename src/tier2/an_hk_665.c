#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005de494*/ extern float k_f_3_1415927_5de494;
extern double cos(double);
/* interp_cosine */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(a, b, t): cosine interpolation" */

/* Eased interpolation from start to end: blend = (1 - cos(pi * t)) / 2 is 0 at t = 0 and 1 at t = 1,
 * so the value moves slowly at both ends and fastest in the middle. t is normally 0..1 (inferred).
 * Returns start * (1 - blend) + end * blend. */
float10 __cdecl interp_cosine(float start,float end,float t)

{
  float10 blend;
  
  blend = (float10)cos((float10)k_f_3_1415927_5de494 * (float10)t);
  blend = ((float10)float_1_0 - blend) * (float10)k_f_0_5;
  return blend * (float10)end + ((float10)float_1_0 - blend) * (float10)start;
}


