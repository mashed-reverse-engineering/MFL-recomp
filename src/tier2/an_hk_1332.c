#include "mfl_tier2.h"
/*005dda20*/ extern float k_f_4_2949673e9;
/* gap_5d9a20 */

/* no context */

char *gap_5d9a20(char *param_1,char *unused_2,int *param_3)

{
  char *iVar1;
  float fVar2;
  
  iVar1 = *(char **)(param_1 + 0x20);
  fVar2 = (float)*param_3;
  if (*param_3 < 0) {
    fVar2 = fVar2 + k_f_4_2949673e9;
  }
  *(float *)(iVar1 + 0x60) = fVar2;
  *(uint *)(iVar1 + 0x5c) = *(uint *)(iVar1 + 0x5c) | 0x20;
  return param_1;
}


