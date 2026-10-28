#include "mfl_tier2.h"
/*005e64a4*/ extern float k_f_0;
/* gap_5d7af0 */

/* no context */

/* Initializes out_scale_block (scale 1,1,1; 0,0,0; 1.0) and out_zero_vec to defaults; returns k_f_0 (inferred). */
double gap_5d7af0(char *unused_1,undefined4 *out_scale_block,undefined4 *out_zero_vec)

{
  double zero_ret;
  
  zero_ret = k_f_0;
  *out_scale_block = 0x3f800000;
  out_scale_block[1] = 0x3f800000;
  out_scale_block[2] = 0x3f800000;
  out_zero_vec[2] = 0;
  out_zero_vec[1] = 0;
  *out_zero_vec = 0;
  out_scale_block[5] = 0;
  out_scale_block[4] = 0;
  out_scale_block[3] = 0;
  out_scale_block[6] = 0x3f800000;
  return zero_ret;
}


