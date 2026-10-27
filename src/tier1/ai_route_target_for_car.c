#include "mfl_tier2.h"

#define MATRIX_POS_X_OFFSET 0x30
#define MATRIX_POS_Z_OFFSET 0x38
#define USE_BOT_HINT 1
#define UNKNOWN_PARAM 0

/*0044d2a0*/ extern int ai_nearest_route_point();
/*0047b520*/ extern int car_current_matrix();


/* ai_route_target_for_car */

/* "(bot, out, car): ai_nearest_route_point from the car position with the per bot hint" */
/* Finds the nearest route target point for a car using its position and bot hint. */
void ai_route_target_for_car(undefined4 bot,undefined4 out_target,undefined4 car)
{
  struct { char *car_matrix; undefined4 car_pos_x; undefined4 car_pos_z; } lcl_;
#define car_matrix lcl_.car_matrix
#define car_pos_x lcl_.car_pos_x
#define car_pos_z lcl_.car_pos_z
  
  /* Retrieve car's current transformation matrix */
  car_current_matrix(&car_matrix,car);
  /* Extract X and Z coordinates of the car position */
  car_pos_x = *(undefined4 *)(car_matrix + MATRIX_POS_X_OFFSET);
  car_pos_z = *(undefined4 *)(car_matrix + MATRIX_POS_Z_OFFSET);
  /* Query nearest route point using car position and bot hint (last parameter unknown) */
  ai_nearest_route_point(bot,&car_pos_x,out_target,car,USE_BOT_HINT,UNKNOWN_PARAM);
  return;
}
