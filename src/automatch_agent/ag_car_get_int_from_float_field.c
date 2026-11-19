#include "mfl_tier2.h"
/*008b10c0*/ extern undefined4 g_car_8b10c0;

enum { CAR_COUNT = 16, CAR_STRUCT_STRIDE = 0xd04 };

/* car_get_int_from_float_field */

/* "_ftol of a car float field into the output" */

/* Stores (int) of the float field at car offset 0x9E0 (not yet named) for car_index into *out_value.
   Returns 1 on success, 0 if car_index is out of range. */
undefined4 car_get_int_from_float_field(int *out_value,uint car_index)

{

  uint idx = car_index;
  if (idx >= CAR_COUNT) {
    return 0;
  }
  *out_value = (int)*(float *)((char *)&g_car_8b10c0 + idx * CAR_STRUCT_STRIDE);
  return 1;
}
