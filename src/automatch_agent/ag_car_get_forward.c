#include "mfl_tier2.h"
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*0062ac80*/ extern undefined g_weapon_62ac80;
/*008b06e0*/ extern struct Car player_car[16];

enum { CAR_COUNT = 16 };

/* car_get_forward */

/* "(&out, car): transforms (0,0,1) by the current matrix, stored at +0x9D4" */

/* Writes the car's forward vector to out_vec. Returns 1 on success, 0 if car_index is out of range. */
undefined4 car_get_forward(float *out_vec,uint car_index)

{
  if (car_index >= CAR_COUNT) {
    return 0;
  }
  RwV3dTransformVectors
            ((RwV3d *)player_car[car_index].forward,(RwV3d *)&g_weapon_62ac80,1,
             (RwMatrix *)player_car[car_index].matrices);
  *out_vec = player_car[car_index].forward[0];
  out_vec[1] = player_car[car_index].forward[1];
  out_vec[2] = player_car[car_index].forward[2];
  return 1;
}
