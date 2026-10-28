#include "mfl_tier2.h"
/*005dfdf8*/ extern float k_f_3_4028235e38;
/*005f6550*/ extern float k_f_n3_4028235e38;
/*005f8f54*/ extern float k_f_0_999995;
/*005f8f58*/ extern float k_f_n0_999995;
/* gap_5d7cf0 */

/* no context */

/* Writes -FLT_MAX to *out_neg_flag if vec->z > -0.999995 else 0, and +FLT_MAX to *out_pos_flag if vec->z < 0.999995 else 0 (inferred; vec+8 is a float component); returns 1. */
undefined4 gap_5d7cf0(char *unused_1,uint vec,float *out_neg_flag,float *out_pos_flag)

{
  int vec_addr;
  
  vec_addr = vec;
  vec = (uint)(k_f_n0_999995 < *(float *)(vec + 8));
  *out_neg_flag = (float)(int)vec * k_f_n3_4028235e38;
  vec = (uint)(*(float *)(vec_addr + 8) < k_f_0_999995);
  *out_pos_flag = (float)(int)vec * k_f_3_4028235e38;
  return 1;
}


