#include "mfl_tier2.h"
/*0047b520*/ extern int car_current_matrix(RwMatrix **out, int car);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd330*/ extern float float_1_0;
/*005ddb10*/ extern float k_f_0_99;
/*005e64a4*/ extern float k_f_0;
/*00652d08*/ extern undefined g_course_update_652d08;
/* car wobble state at g_course_update_652d08, 0x28 bytes per car: a damped oscillation per axis */
typedef struct {
  float yaw_duration;
  float roll_duration;
  float yaw_time;
  float yaw_gain;
  float roll_time;
  float roll_gain;
  float yaw_fraction;
  float roll_fraction;
  int yaw_active;
  int roll_active;
} CarWobble;
#define g_car_wobble ((CarWobble *)&g_course_update_652d08)
#define WOBBLE_DONE_FRACTION k_f_0_99
/* car_wobble_tilt_update */

/* "damped yaw/roll oscillation of the car matrix (tables at 0x652d08, stride 0x28) around the car
   origin" */

void __cdecl car_wobble_tilt_update(int car)

{
  CarWobble *wobble;
  float yaw;
  float roll;
  double fraction;
  RwMatrix *matrix;
  RwV3d offset;
  
  wobble = g_car_wobble + car;
  roll = 0.0;
  if ((wobble->yaw_active != 0) || (wobble->roll_active != 0)) {
    if (wobble->yaw_duration == k_f_0) {
      yaw = 0.0;
    }
    else {
      fraction = (double)wobble->yaw_time / (double)wobble->yaw_duration;
      wobble->yaw_fraction = (float)fraction;
      yaw = (float)(((double)float_1_0 - fraction) * (double)wobble->yaw_gain);
      wobble->yaw_time = yaw + wobble->yaw_time;
    }
    if (wobble->roll_duration == k_f_0) {
      yaw = 0.0;      /* sic: the original clears the yaw here, not the roll */
    }
    else {
      fraction = (double)wobble->roll_time / (double)wobble->roll_duration;
      wobble->roll_fraction = (float)fraction;
      roll = (float)(((double)float_1_0 - fraction) * (double)wobble->roll_gain);
      wobble->roll_time = roll + wobble->roll_time;
    }
    car_current_matrix(&matrix,car);
    offset.x = matrix->pos.x * -1.0f;
    offset.y = matrix->pos.y * -1.0f;
    offset.z = matrix->pos.z * -1.0f;
    RwMatrixTranslate(matrix,&offset,rwCOMBINEPOSTCONCAT);
    RwMatrixRotate(matrix,&matrix->up,yaw,rwCOMBINEPOSTCONCAT);
    RwMatrixRotate(matrix,&matrix->at,roll,rwCOMBINEPOSTCONCAT);
    offset.x = offset.x * -1.0f;
    offset.y = offset.y * -1.0f;
    offset.z = offset.z * -1.0f;
    RwMatrixTranslate(matrix,&offset,rwCOMBINEPOSTCONCAT);
    if (WOBBLE_DONE_FRACTION <= wobble->yaw_fraction) {
      wobble->yaw_duration = 0.0;
      wobble->yaw_gain = 0.0;
      wobble->yaw_time = 0.0;
      wobble->yaw_fraction = 0.0;
      wobble->yaw_active = 0;
    }
    if (WOBBLE_DONE_FRACTION <= wobble->roll_fraction) {
      wobble->roll_duration = 0.0;
      wobble->roll_gain = 0.0;
      wobble->roll_time = 0.0;
      wobble->roll_fraction = 0.0;
      wobble->roll_active = 0;
    }
  }
  return;
}
