#include "mfl_tier2.h"
/*0047b520*/ extern int car_current_matrix();
/*0047b590*/ extern int car_get_forward();
/*004b3284*/ extern double crt_acos_body(double p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005ddacc*/ extern float k_f_360;
/*005e64a4*/ extern float k_f_0;

/* radians to degrees: 180 / pi */
#define RAD_TO_DEG (57.295780181884766)
/* car world position inside its RwMatrix (translation row; inferred from RwMatrix layout) */
#define CAR_MATRIX_POS_X (0x30)
#define CAR_MATRIX_POS_Z (0x38)

/* ai_steer_towards_point */

/* Steering angle in degrees, 0..360: how far the car must turn from its forward heading to face the world point
 * (target_x, target_z) in the XZ plane. Each heading is the angle from +X (acos of the X component, sign from Z,
 * inferred). Both headings are converted to degrees, then the difference is wrapped into 0..360.
 * target_x is reused as scratch: after the first heading it holds the target heading, not the X coordinate.
 * (was: "(car, x, z): normalised direction from the car to (x, z) in the XZ plane") */

double ai_steer_towards_point(undefined4 car,float target_x,float target_z)

{
  double angle;
  double cos_angle;
  struct {
    char *car_matrix;
    RwV3d dir;
  } frame;

  /* direction from the car's position to the target in the XZ plane (Z = car_z - target_z, sign flipped as in the original) */
  car_current_matrix(&frame.car_matrix,car);
  frame.dir.x = target_x - *(float *)(frame.car_matrix + CAR_MATRIX_POS_X);
  frame.dir.y = 0.0;
  frame.dir.z = -(target_z - *(float *)(frame.car_matrix + CAR_MATRIX_POS_Z));
  RwV3dNormalize(&frame.dir,&frame.dir);
#define DIR_COS (cos_angle + (double)frame.dir.z * k_f_0)
  /* heading of the target direction: clamp cos to [-1, 1], arccos, degrees (crt_acos_body = acos, inferred) */
  cos_angle = (double)frame.dir.y * k_f_0 + frame.dir.x;
  angle = (double)crt_acos_body((double)(DIR_COS < k_f_n1 ? (double)k_f_n1 : (float_1_0 < DIR_COS ? (double)float_1_0 : DIR_COS)));
  angle = angle * (double)RAD_TO_DEG;
  target_x = (float)angle;
  /* negative Z side: mirror the heading */
  if (frame.dir.z - frame.dir.x * k_f_0 < k_f_0) {
    angle = -angle;
    target_x = (float)angle;
  }
  /* bring the heading up into the positive range (loop adds 360 while <= 0, inferred) */
  if (angle <= k_f_0) {
    do {
      angle = angle + (double)k_f_360;
    } while (angle <= k_f_0);
    target_x = (float)angle;
  }
  /* second heading: the car's forward vector (car_get_forward), same steps as above */
  car_get_forward(&frame.dir,car);
  frame.dir.z = -frame.dir.z;
  frame.dir.y = 0.0;
  RwV3dNormalize(&frame.dir,&frame.dir);
  cos_angle = (double)frame.dir.y * k_f_0 + frame.dir.x;
  angle = (double)crt_acos_body((double)(DIR_COS < k_f_n1 ? (double)k_f_n1 : (float_1_0 < DIR_COS ? (double)float_1_0 : DIR_COS)));
#undef DIR_COS
  angle = angle * (double)RAD_TO_DEG;
  if (frame.dir.z - frame.dir.x * k_f_0 < k_f_0) {
    angle = -angle;
  }
  if (angle <= k_f_0) {
    do {
      angle = angle + (double)k_f_360;
    } while (angle <= k_f_0);
  }
  /* turn needed = target heading - forward heading */
  angle = (double)target_x - angle;
  if (angle <= k_f_0) {
    do {
      angle = angle + (double)k_f_360;
    } while (angle <= k_f_0);
  }
  /* wrap the difference into 0..360 */
  if (angle < k_f_0) {
    do {
      angle = angle + (double)k_f_360;
    } while (angle < k_f_0);
  }
  if ((double)k_f_360 < angle) {
    do {
      angle = angle - (double)k_f_360;
    } while ((double)k_f_360 < angle);
  }
  return angle;
}
