#include "mfl_tier2.h"
/*0056fd20*/ extern float __cdecl FUN_0056fd20(void *a1, void *a2);
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005ddc28*/ extern float k_f_0;
/*005de490*/ extern float k_f_0_02;
/*005f5a74*/ extern float k_f_0_9998;
extern double fabs(double);
/* gap_5700b0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* no context */

void gap_5700b0(char *param_1,float *param_2,float *param_3)

{
  float fVar1;
  float fVar2;
  struct {
    float fStack_c;
    float fStack_8;
    float fStack_4;
  } frame;
  
  frame.fStack_c = *param_2;
  frame.fStack_8 = param_2[1];
  fVar1 = param_2[2];
  fVar2 = fabs(fVar1);
  if (!(fVar2 <= k_f_0_9998)) {      /* NaN takes the axis branch (original: fcom / test ah,0x41 / jp) */
    frame.fStack_c = 0.0;
    frame.fStack_8 = 0.0;
    frame.fStack_4 = float_1_0;
    if (fVar1 < k_f_0) {
      frame.fStack_4 = k_f_n1;
    }
    frame.fStack_4 = frame.fStack_4 * *(float *)(param_1 + 0x40);
  }
  else {
    frame.fStack_4 = 0.0;
    if (!(fVar2 > k_f_0_02)) {      /* NaN: the first branch (original: and eax,0x4100 / jne) */
      FUN_0056fd20(&frame.fStack_c,&frame.fStack_c);
      frame.fStack_c = *(float *)(param_1 + 0x44) * frame.fStack_c;
      frame.fStack_8 = *(float *)(param_1 + 0x44) * frame.fStack_8;
      frame.fStack_4 = *(float *)(param_1 + 0x44) * frame.fStack_4;
    }
    else {
      FUN_0056fd20(&frame.fStack_c,&frame.fStack_c);
      frame.fStack_c = *(float *)(param_1 + 0x44) * frame.fStack_c;
      frame.fStack_8 = *(float *)(param_1 + 0x44) * frame.fStack_8;
      if (!(fVar1 < k_f_0)) {      /* NaN counts as >= 0 (original: fcomp / test ah,5 / jp) */
        frame.fStack_4 = float_1_0 * *(float *)(param_1 + 0x40);
      }
      else {
        frame.fStack_4 = k_f_n1 * *(float *)(param_1 + 0x40);
      }
    }
  }
  *param_3 = frame.fStack_c;
  param_3[1] = frame.fStack_8;
  param_3[2] = frame.fStack_4;
  return;
}


