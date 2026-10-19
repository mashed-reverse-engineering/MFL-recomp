#include "mfl_tier2.h"
/*004b39c4*/ extern double FUN_004b39c4(double p0);
/*005ddaec*/ extern float k_f_57_29578;
/*005dfe18*/ extern float k_f_n1_5707964;
/*005dfe1c*/ extern float k_f_0_999999;
extern double atan2(double, double);
extern double fabs(double);
/* matrix_to_euler_degrees */

void matrix_to_euler_degrees(float *param_1,float *param_2)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fa;
  float a;
  double r4;
  double r5;
  
  if (param_2 != (float *)0x0) {
    fVar1 = *param_2;
    fVar2 = param_2[1];
    fVar3 = param_2[2];
    fa = (float)fabs(fVar3);
    if (fa <= k_f_0_999999) {
      a = (float)atan2(param_2[6],param_2[10]);
      r4 = FUN_004b39c4(-fVar3);
      r5 = atan2(fVar2,fVar1);
      *param_1 = a * k_f_57_29578;
    }
    else {
      r4 = (fVar3 / fa) * k_f_n1_5707964;
      *param_1 = 0.0;
      r5 = atan2(-param_2[4],-(param_2[8] * fVar3));
    }
    param_1[1] = (float)(r4 * k_f_57_29578);
    param_1[2] = (float)(r5 * k_f_57_29578);
  }
  return;
}
