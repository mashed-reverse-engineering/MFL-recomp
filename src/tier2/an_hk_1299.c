#include "mfl_tier2.h"
/*005ec3d4*/ extern float k_f_0_9999;
/*005f8f4c*/ extern float k_f_n0_9999;
/* gap_5d7b30 */

/* no context */

float * gap_5d7b30(char *param_1,char *param_2,char *unused_3,float *param_4)

{
  float fVar1;
  
  fVar1 = *(float *)(param_1 + 0x4c);
  *param_4 = 1.7014117e+38;
  param_4[1] = 1.7014117e+38;
  param_4[2] = 1.7014117e+38;
  param_4[4] = -1.7014117e+38;
  param_4[5] = -1.7014117e+38;
  param_4[6] = -1.7014117e+38;
  if (param_2 != 0) {
    if (k_f_0_9999 < *(float *)(param_2 + 0x20)) {
      *param_4 = fVar1 + *(float *)(param_2 + 0x30);
      return param_4;
    }
    if (*(float *)(param_2 + 0x20) < k_f_n0_9999) {
      param_4[4] = *(float *)(param_2 + 0x30) - fVar1;
      return param_4;
    }
    if (k_f_0_9999 < *(float *)(param_2 + 0x24)) {
      param_4[1] = fVar1 + *(float *)(param_2 + 0x34);
      return param_4;
    }
    if (*(float *)(param_2 + 0x24) < k_f_n0_9999) {
      param_4[5] = *(float *)(param_2 + 0x34) - fVar1;
      return param_4;
    }
    if (!(*(float *)(param_2 + 0x28) > k_f_0_9999)) {      /* NaN takes this branch in the original (and eax,0x4100; jne) */
      if (*(float *)(param_2 + 0x28) < k_f_n0_9999) {
        param_4[6] = *(float *)(param_2 + 0x38) - fVar1;
      }
      return param_4;
    }
    fVar1 = fVar1 + *(float *)(param_2 + 0x38);
  }
  param_4[2] = fVar1;
  return param_4;
}


