#include "mfl_tier2.h"
/*0056fd20*/ extern float __cdecl FUN_0056fd20(void *a1, void *a2);
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005e64a4*/ extern float k_f_0;
/*005de490*/ extern float k_f_0_02;
/*005f5a74*/ extern float k_f_0_9998;
extern double fabs(double);
/* gap_5700b0 */

/* no context */

/* Builds a vector from axis_in (obj+0x40 / obj+0x44 are scale factors): if |z| > 0.9998 result is (0,0,+-obj[0x40]) by the sign of z, else the xy part is normalised and scaled by obj[0x44] (z scaled by obj[0x40] when |z| > 0.02); writes vec_out (inferred). */
void gap_5700b0(char *obj,float *axis_in,float *vec_out)

{
  float axis_z;
  float abs_in_z;
  struct {
    float fStack_c;
    float fStack_8;
    float fStack_4;
  } frame;
  
  frame.fStack_c = *axis_in;
  frame.fStack_8 = axis_in[1];
  axis_z = axis_in[2];
  abs_in_z = fabs(axis_z);
  if (!(abs_in_z <= k_f_0_9998)) {      /* NaN takes the axis branch (original: fcom / test ah,0x41 / jp) */
    frame.fStack_c = 0.0;
    frame.fStack_8 = 0.0;
    frame.fStack_4 = float_1_0;
    if (axis_z < k_f_0) {
      frame.fStack_4 = k_f_n1;
    }
    frame.fStack_4 = frame.fStack_4 * *(float *)(obj + 0x40);
  }
  else {
    frame.fStack_4 = 0.0;
    if (!(abs_in_z > k_f_0_02)) {      /* NaN: the first branch (original: and eax,0x4100 / jne) */
      FUN_0056fd20(&frame.fStack_c,&frame.fStack_c);
      frame.fStack_c = *(float *)(obj + 0x44) * frame.fStack_c;
      frame.fStack_8 = *(float *)(obj + 0x44) * frame.fStack_8;
      frame.fStack_4 = *(float *)(obj + 0x44) * frame.fStack_4;
    }
    else {
      FUN_0056fd20(&frame.fStack_c,&frame.fStack_c);
      frame.fStack_c = *(float *)(obj + 0x44) * frame.fStack_c;
      frame.fStack_8 = *(float *)(obj + 0x44) * frame.fStack_8;
      if (!(axis_z < k_f_0)) {      /* NaN counts as >= 0 (original: fcomp / test ah,5 / jp) */
        frame.fStack_4 = float_1_0 * *(float *)(obj + 0x40);
      }
      else {
        frame.fStack_4 = k_f_n1 * *(float *)(obj + 0x40);
      }
    }
  }
  *vec_out = frame.fStack_c;
  vec_out[1] = frame.fStack_8;
  vec_out[2] = frame.fStack_4;
  return;
}


