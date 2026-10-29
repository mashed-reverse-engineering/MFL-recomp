#include "mfl_tier2.h"
/*005e64a4*/ extern float k_f_0;
/*005de1b4*/ extern float k_f_0_0625;
/*008b0888*/ extern char player_car_0__wheels_0__field40_0x40[];
#define MAX_CARS (0x10)
#define WHEELS_PER_CAR (4)
#define WHEEL_STRIDE (0xc4)
#define CAR_STRIDE (0xd04)
#define WHEEL_SAMPLE_OFFSET (0x8)
#define WHEEL_SAMPLE_PASSES (2)
/* car_get_wheel_average */

/* Averages the 16 float samples of one wheel of one car and stores the result in *out_average.
 * car_index: 0..MAX_CARS-1, wheel_index: 0..WHEELS_PER_CAR-1.
 * Returns 1 on success, 0 if an index is out of range (*out_average is then not written).
 * (was: "(out, car, wheel): average of the 16 per wheel floats at Car + 0x890 (stride 0xC4)";
 * the Car + 0x890 claim is not verifiable from this body) */

undefined4 car_get_wheel_average(float *out_average,uint car_index,uint wheel_index)

{
  float wheel_sum;
  float *sample;
  int remaining_passes;

  /* Bounds check on both indices. */
  if ((car_index < MAX_CARS) && (wheel_index < WHEELS_PER_CAR)) {
    /* Sample window for this wheel: sums 2 passes of 8 floats (16 samples). */
    remaining_passes = WHEEL_SAMPLE_PASSES;
    sample =(float *)(wheel_index * WHEEL_STRIDE + car_index * CAR_STRIDE + ((char *)&player_car_0__wheels_0__field40_0x40 + WHEEL_SAMPLE_OFFSET));
    wheel_sum = k_f_0;
    /* Each pass adds 8 samples and advances by 8 floats. */
    do {
      remaining_passes = remaining_passes + -1;
      wheel_sum = wheel_sum + sample[-1] + *sample + sample[1] + sample[2] + sample[3] + sample[4] +
              sample[5] + sample[6];
      sample = sample + 8;
    } while (remaining_passes != 0);
    *out_average = wheel_sum * k_f_0_0625;
    return 1;
  }
  return 0;
}


