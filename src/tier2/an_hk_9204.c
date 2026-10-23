#include "mfl_tier2.h"
extern char LAB_0047b420[];
/*00549290*/ extern int RpCollisionWorldForAllIntersections();
/*005dd32c*/ extern float k_f_3;
/*005dd814*/ extern float k_f_6;
/*005e64a4*/ extern float k_f_0;
/* car_probe_ground_distance */

/* "(x, y, z, world): collision ray 3 units up / down, returns the distance to the ground minus 3"
    */

double car_probe_ground_distance(float param_1,float param_2,float param_3,char *param_4)

{
  struct {
    float start_x;
    float start_y;
    float start_z;
    float end_x;
    float end_y;
    float end_z;
    int flag;
  } ray;

  ray.end_y = param_2 - 3.0f;
  ray.end_x = param_1;
  ray.end_z = param_3;
  ray.start_y = param_2 + 3.0f;
  ray.start_x = param_1;
  ray.start_z = param_3;
  ray.flag = 1;
  param_1 = 6.0f;      /* the out value reuses the x parameter slot */
  RpCollisionWorldForAllIntersections(*(undefined4 *)(param_4 + 8),&ray,&LAB_0047b420,&param_1);
  if (param_1 != k_f_6) {
    return (double)param_1 - (double)k_f_3;
  }
  return k_f_0 - (double)k_f_3;
}
