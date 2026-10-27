#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*005dd628*/ extern float k_f_0_25;
/*005dd9c4*/ extern float k_f_0_15;
/*005dda24*/ extern float k_f_0_75;
/*005dfd78*/ extern float k_f_1_05;
/* course_storm_cb1_sub_4aaea0 */

/* single caller course_storm_cb1_sub_4ab160 */

void course_storm_cb1_sub_4aaea0(float *param_1,float param_2,short *param_3)

{
  float fbits1;
  float fbits2;
  float fbits3;
  float fbits4;
  int iVar1;
  double fVar2;
  
  fVar2 = -(double)random_range((fbits1 = param_2 * k_f_0_15, *(int *)&fbits1),(fbits2 = param_2 * k_f_0_25, *(int *)&fbits2));
  iVar1 = 0;
  do {
    if (*param_3 == 0) {
      *param_3 = 1;
      param_3[1] = 0;
      *(float *)(param_3 + 8) = (float)(fVar2 * (double)*param_1);
      *(float *)(param_3 + 0xc) = (float)(fVar2 * (double)param_1[2]);
      fVar2 = (double)random_range((fbits3 = param_2 * k_f_0_75, *(int *)&fbits3),(fbits4 = param_2 * k_f_1_05, *(int *)&fbits4));
      *(float *)(param_3 + 10) = (float)fVar2;
      *(float *)(param_3 + 2) = param_1[0xc];
      *(float *)(param_3 + 4) = param_1[0xd];
      *(float *)(param_3 + 6) = param_1[0xe];
      return;
    }
    param_3 = param_3 + 0x10;
    iVar1 = iVar1 + 1;
  } while (iVar1 < 0x200);
  return;
}


