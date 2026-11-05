/* rwa_soft_gain_ramp_start_005d7e30 (0x005d7e30): software mixer: start a 64-sample gain ramp of a stereo voice */
#include "mfl_tier2.h"

#define RAMP_SAMPLES 64

/* ramp[1] / ramp[2] = current left / right gain (8.8 fixed point), ramp[3] / ramp[4] = step per sample towards the new
   gains (8 bit integers), ramp[0] = samples left in the ramp (0 when nothing moves).  Returns ramp[0]. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_soft_gain_ramp_start_005d7e30(int *ramp, int left, int right)
{
  int step_left = ((left << 8) - ramp[1]) / RAMP_SAMPLES;
  int step_right = ((right << 8) - ramp[2]) / RAMP_SAMPLES;

  ramp[3] = step_left;
  ramp[4] = step_right;
  ramp[0] = (step_left != 0 || step_right != 0) ? RAMP_SAMPLES : 0;
  return ramp[0];
}
