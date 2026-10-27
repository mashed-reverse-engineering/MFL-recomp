#include "mfl_tier2.h"
/*004222e0*/ extern int vehicle_get_matrix_ltm_b();
/*00424710*/ extern undefined __cdecl car_damage_at_local_point(undefined4 p0, undefined4 p1, RwV3d *p2);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/* car_damage_at_world_point */
/* "(car, amount, world position): transforms the point into car space with the inverse matrix and
   calls car_damage_at_local_point" */
/* Parameters: car = car handle (inferred), amount = damage amount (inferred), world_point = point in world space.
   Returns nothing. Hands the point to car_damage_at_local_point in the car's local space. */
void car_damage_at_world_point(undefined4 car,undefined4 amount,RwV3d *world_point)
{
  struct {
    RwV3d local_point;
    RwMatrix car_matrix;
    RwMatrix inverse_matrix;
  } frame;

  vehicle_get_matrix_ltm_b(car, &frame.car_matrix);
  RwMatrixInvert(&frame.inverse_matrix,&frame.car_matrix);
  RwV3dTransformPoints(&frame.local_point,world_point,1,&frame.inverse_matrix);
  car_damage_at_local_point(car,amount,&frame.local_point);
  return;
}
