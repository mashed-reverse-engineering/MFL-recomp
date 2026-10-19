#include "mfl_tier2.h"
/*005dd340*/ extern float k_f_0_5;
/*005df300*/ extern float k_f_0_333333;
/*008b06e0*/ extern struct Car player_car[16];
#define MAX_CAR_INDEX (0xf) /* last valid index into player_car (16 cars, 0..15) */
/* car_set_bounds */

/* fills the 8 box corner points (bounds_points, +0x90) and the 6 derived points (body_points, +0xf0) of a car from its min/max box.
   car_index: index into player_car, must be <= MAX_CAR_INDEX
   box: 6 floats {min x, min y, min z, max x, max y, max z} (inferred from how the corners use the indices)
   returns 1 on success, 0 if car_index is out of range */

static float rz1, rz2, rz3, rz4;
static float r2, r3, r6, r11, r12, r15;
undefined4 car_set_bounds(uint car_index,float *box)

{
  struct Car *car;
  float mid_x, mid_y, m2, sum_a, sum_b, t2;

  if (MAX_CAR_INDEX < car_index) {
    return 0;
  }
  car = &player_car[car_index];
  /* 8 box corners, 3 floats each, at bounds_points[0..23] (corner order inferred) */
  car->bounds_points[0] = *box;
  car->bounds_points[1] = box[4];
  car->bounds_points[2] = box[2];
  car->bounds_points[3] = box[3];
  mid_x = car->bounds_points[0] + car->bounds_points[3];
  car->bounds_points[4] = box[4];
  car->bounds_points[5] = box[2];
  mid_y = car->bounds_points[4] + car->bounds_points[1];
  car->bounds_points[6] = *box;
  car->bounds_points[7] = box[4];
  car->bounds_points[8] = box[5];
  car->bounds_points[9] = box[3];
  car->bounds_points[10] = box[4];
  car->bounds_points[11] = box[5];
  car->bounds_points[12] = *box;
  car->bounds_points[13] = box[1];
  car->bounds_points[14] = box[2];
  car->bounds_points[15] = box[3];
  car->bounds_points[16] = box[1];
  car->bounds_points[17] = box[2];
  car->bounds_points[18] = *box;
  car->bounds_points[19] = box[1];
  car->bounds_points[20] = box[5];
  car->bounds_points[21] = box[3];
  car->bounds_points[22] = box[1];
  car->bounds_points[23] = box[5];
  /* body_points[0..2]: midpoint of the edge between corners 0 and 1 (inferred) */
  r2 = car->bounds_points[2] + car->bounds_points[5];
  car->body_points[0] = mid_x * k_f_0_5;
  car->body_points[1] = mid_y * k_f_0_5;
  car->body_points[2] = r2 * k_f_0_5;

  /* body_points[3..5]: points 1/3 of the way from corner 1 towards corner 3, weighted 2:1 (inferred) */
  sum_b = car->bounds_points[4] + car->bounds_points[4] + car->bounds_points[10];
  rz1 = car->bounds_points[5] + car->bounds_points[5];
  r3 = car->bounds_points[3] + car->bounds_points[3] + car->bounds_points[9];
  car->body_points[4] = sum_b * k_f_0_333333;
  car->body_points[5] = (rz1 + car->bounds_points[11]) * k_f_0_333333;
  car->body_points[3] = r3 * k_f_0_333333;

  /* body_points[6..8]: points 1/3 of the way from corner 3 towards corner 1, weighted 2:1 (inferred) */
  sum_b = car->bounds_points[10] + car->bounds_points[10] + car->bounds_points[4];
  rz2 = car->bounds_points[11] + car->bounds_points[11];
  r6 = car->bounds_points[9] + car->bounds_points[9] + car->bounds_points[3];
  car->body_points[7] = sum_b * k_f_0_333333;
  car->body_points[8] = (rz2 + car->bounds_points[5]) * k_f_0_333333;
  car->body_points[6] = r6 * k_f_0_333333;
  /* body_points[9..11]: edge midpoints between corners 2 and 3 (inferred) */
  sum_a = car->bounds_points[9] + car->bounds_points[6];
  sum_b = car->bounds_points[10] + car->bounds_points[7];

  r11 = car->bounds_points[11] + car->bounds_points[8];
  car->body_points[9] = sum_a * k_f_0_5;
  car->body_points[10] = sum_b * k_f_0_5;
  car->body_points[11] = r11 * k_f_0_5;

  /* body_points[12..14]: 1/3 points weighted 2:1 from corner 2 towards corner 0 (inferred) */
  sum_b = car->bounds_points[7] + car->bounds_points[7] + car->bounds_points[1];
  rz3 = car->bounds_points[8] + car->bounds_points[8];
  r12 = car->bounds_points[6] + car->bounds_points[6] + car->bounds_points[0];
  car->body_points[13] = sum_b * k_f_0_333333;
  car->body_points[14] = (rz3 + car->bounds_points[2]) * k_f_0_333333;
  car->body_points[12] = r12 * k_f_0_333333;

  /* body_points[15..17]: 1/3 points weighted 2:1 from corner 0 towards corner 2 (inferred) */
  sum_b = car->bounds_points[1] + car->bounds_points[1] + car->bounds_points[7];
  rz4 = car->bounds_points[2] + car->bounds_points[2];
  r15 = car->bounds_points[0] + car->bounds_points[0] + car->bounds_points[6];
  car->body_points[16] = sum_b * k_f_0_333333;
  car->body_points[17] = (rz4 + car->bounds_points[8]) * k_f_0_333333;
  car->body_points[15] = r15 * k_f_0_333333;
  return 1;
}
