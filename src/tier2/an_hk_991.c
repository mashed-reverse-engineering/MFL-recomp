#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count();
/*004222a0*/ extern int vehicle_part_matrix(int car, int part, RwMatrix *out);
/*0045ccb0*/ extern int course_zone_contains_matrix(void *zone, RwMatrix *a, RwMatrix *b);
/*0047a470*/ extern int car_set_wheel_contact(void *state, int car, int a, int b);
/*0047a5b0*/ extern int car_add_velocity();
/*0047b520*/ extern int car_current_matrix(RwMatrix **out, int car);
/*00491ab0*/ extern int car_enter_rwp_mode();
/*00492b20*/ extern int car_apply_rwp_impulse(int car, RwV3d *impulse, RwV3d *position);
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*0060d9c4*/ extern undefined g_car_60d9c4;
/*008bf050*/ extern undefined4 g_car_8bf050;
/* course_zone_cars_check_and_push */

/* "per car: zone test, on exit adds the movement velocity and tests the four wheel matrices
   (0x21..0x24)" */

#define WHEEL_PART_FIRST (0x21)
#define WHEEL_PART_LAST (0x24)
#define ZONE_INSIDE_FLAGS (0xb4)      /* per car: last zone test result */
#define ZONE_MOVEMENT (0xa8)

void course_zone_cars_check_and_push(char *param_1, undefined4 st_arg2)

{
  int car;
  int car_count;
  int part;
  int zone_result;
  int *inside_flag;
  RwMatrix *car_matrix;
  RwV3d impulse;
  RwMatrix wheel_matrix;
  
  car_count = get_player_count();
  car = 0;
  impulse.x = 0.0;
  impulse.y = -75.0f;
  impulse.z = 0.0;
  if (0 < car_count) {
    inside_flag = (int *)(param_1 + ZONE_INSIDE_FLAGS);
    do {
      car_current_matrix(&car_matrix,car);
      zone_result = course_zone_contains_matrix(param_1,car_matrix,car_matrix);
      if (zone_result == 0) {
        (&g_car_8bf050)[car] = 0;
        if (*inside_flag != 0) {
          FID_conflict__wprintf
                    ("movement is (%f, %f, %f)\n",(double)*(float *)(param_1 + ZONE_MOVEMENT),
                     (double)*(float *)(param_1 + ZONE_MOVEMENT + 4),(double)*(float *)(param_1 + ZONE_MOVEMENT + 8));
          car_add_velocity((float *)(param_1 + ZONE_MOVEMENT),car);
          for (part = WHEEL_PART_FIRST; part <= WHEEL_PART_LAST; part = part + 1) {
            vehicle_part_matrix(car,part,&wheel_matrix);
            if (course_zone_contains_matrix(param_1,&wheel_matrix,&wheel_matrix) == 0) {
              car_apply_rwp_impulse(car,&impulse,&wheel_matrix.pos);
            }
          }
          car_enter_rwp_mode(car);
        }
        car_set_wheel_contact(&g_car_60d9c4,car,0,1);
      }
      else {
        car_set_wheel_contact(&g_car_60d9c4,car,0,0);
        (&g_car_8bf050)[car] = 1;
      }
      *inside_flag = zone_result;
      inside_flag = inside_flag + 1;
      car = car + 1;
    } while (car < car_count);
  }
  return;
}
