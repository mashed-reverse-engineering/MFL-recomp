#include "mfl_tier2.h"
/*005ddc28*/ extern float k_f_0;
/*005dfdf8*/ extern float k_f_3_4028235e38;
/* gap_5d7c10 */

/* no context */

float * gap_5d7c10(char *unused_1,float *param_2,float *param_3)

{
  *param_3 = *param_2 * k_f_3_4028235e38;
  param_3[1] = param_2[1] * k_f_3_4028235e38;
  if (k_f_0 < param_2[2]) {
    param_3[2] = 0.0;
    return param_3;
  }
  param_3[2] = param_2[2] * k_f_3_4028235e38;
  return param_3;
}


