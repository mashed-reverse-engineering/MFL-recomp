#include "mfl_tier2.h"
/*004b39c4*/ extern double FUN_004b39c4(double p0);
/*005ddaec*/ extern float k_f_57_29578;
/*005dfe18*/ extern float k_f_n1_5707964;
/*005dfe1c*/ extern float k_f_0_999999;
extern double atan2(double, double);
extern double fabs(double);
/* matrix_to_euler_degrees */

void matrix_to_euler_degrees(float *out_euler,float *matrix)

{
  float m00;
  float m01;
  float m02;
  float abs_m02;
  float roll_rad;
  double pitch_rad;
  double yaw_rad;
  
  if (matrix != (float *)0x0) {
    m00 = *matrix;
    m01 = matrix[1];
    m02 = matrix[2];
    abs_m02 = (float)fabs(m02);
    if (abs_m02 <= k_f_0_999999) {
      roll_rad = (float)atan2(matrix[6],matrix[10]);
      pitch_rad = FUN_004b39c4(-m02);
      yaw_rad = atan2(m01,m00);
      *out_euler = roll_rad * k_f_57_29578;
    }
    else {
      pitch_rad = (m02 / abs_m02) * k_f_n1_5707964;
      *out_euler = 0.0;
      yaw_rad = atan2(-matrix[4],-(matrix[8] * m02));
    }
    out_euler[1] = (float)(pitch_rad * k_f_57_29578);
    out_euler[2] = (float)(yaw_rad * k_f_57_29578);
  }
  return;
}
