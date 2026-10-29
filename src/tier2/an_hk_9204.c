#include "mfl_tier2.h"
extern char LAB_0047b420[];
/*00549290*/ extern int RpCollisionWorldForAllIntersections();
/*005dd32c*/ extern float k_f_3;
/*005dd814*/ extern float k_f_6;
/*005e64a4*/ extern float k_f_0;
/* car_probe_ground_distance */

/* "(x, y, z, world): collision ray 3 units up / down, returns the distance to the ground minus 3" (measured from the top of the ray, so 0 when the ground is at y; when the ray hits nothing it returns -3, not +3)
    */

double car_probe_ground_distance(float x_then_out,float y,float z,char *world)

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

  ray.end_y = y - 3.0f;
  ray.end_x = x_then_out;
  ray.end_z = z;
  ray.start_y = y + 3.0f;
  ray.start_x = x_then_out;
  ray.start_z = z;
  ray.flag = 1;
  x_then_out = 6.0f;      /* the out value reuses the x parameter slot */
  RpCollisionWorldForAllIntersections(*(undefined4 *)(world + 8),&ray,&LAB_0047b420,&x_then_out);
  if (x_then_out != k_f_6) {
    return (double)x_then_out - (double)k_f_3;
  }
  return k_f_0 - (double)k_f_3;
}
