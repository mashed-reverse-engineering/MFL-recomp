#include "mfl_tier2.h"
/*008b06e0*/ extern struct Car player_car[16];

enum { CAR_COUNT = 16, MATRIX_FLOATS = 16 };

/* car_set_matrices_from_floats */

/* "(car, float[...]) copies 16 floats to player_car[car].matrices and more" */

/* Copies a 4x4 matrix (16 floats) from matrix_src into both halves of the car's double-buffered matrices.
   Returns 1 on success, 0 if car_index is out of range. */
undefined4 car_set_matrices_from_floats(uint car_index,float *matrix_src)

{
  struct M16 { float f[16]; };

  if (car_index >= CAR_COUNT) {
    return 0;
  }
  *(struct M16 *)player_car[car_index].matrices = *(struct M16 *)matrix_src;
  *(struct M16 *)(player_car[car_index].matrices + MATRIX_FLOATS) = *(struct M16 *)matrix_src;
  return 1;
}
