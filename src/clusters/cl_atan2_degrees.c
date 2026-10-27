// cluster-entries: atan2_degrees
/* Installed cluster atan2_degrees (tools/cluster_install.py from output/cluster_pilot_zy/atan2_degrees.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005e64a4*/ extern float k_f_0;
/*005ddad4*/ extern float k_f_90;
/*005de4ac*/ extern float k_f_270;
/*005ddad0*/ extern float k_f_180;
/*005ddacc*/ extern float k_f_360;
/*005ddc30*/ extern double k_f_n2_5ddc30;
extern double atan2(double y, double x);
#pragma intrinsic(atan2)

/* Angle of the vector (x, y) in degrees, 0..360 (x is the divisor: 90 / 270 for x == 0, 0 for the zero vector). */
double atan2_degrees(float y, float x)
{
  double angle;

  if (x == k_f_0) {
    if (y < k_f_0) {      /* NaN y takes the 90 branch (original: fcomp, test ah,5, jp) */
      angle = k_f_270;
    } else {
      angle = k_f_90;
    }
  } else {
    angle = atan2(y / x, 1.0);
    angle = k_f_180 - -(angle * k_f_n2_5ddc30);
    if (x > k_f_0) {
      angle = angle + k_f_180;
    }
    if (angle >= k_f_360) {
      angle = angle - k_f_360;
    }
  }
  if (y == k_f_0 && x == k_f_0) {
    angle = k_f_0;
  }
  return angle;
}

