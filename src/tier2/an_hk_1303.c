#include "mfl_tier2.h"
/*005dfdf8*/ extern float k_f_3_4028235e38;
/*005f6550*/ extern float k_f_n3_4028235e38;
/*005f8f54*/ extern float k_f_0_999995;
/*005f8f58*/ extern float k_f_n0_999995;
/* gap_5d7cf0 */

/* no context */

undefined4 gap_5d7cf0(char *unused_1,uint param_2,float *param_3,float *param_4)

{
  int iVar1;
  
  iVar1 = param_2;
  param_2 = (uint)(k_f_n0_999995 < *(float *)(param_2 + 8));
  *param_3 = (float)(int)param_2 * k_f_n3_4028235e38;
  param_2 = (uint)(*(float *)(iVar1 + 8) < k_f_0_999995);
  *param_4 = (float)(int)param_2 * k_f_3_4028235e38;
  return 1;
}


