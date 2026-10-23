#include "mfl_tier2.h"
/*004166b0*/ extern double noise_lookup();
/*004b2b48*/ extern int _ftol();
/*005dd340*/ extern float k_f_0_5;
#define NOISE_NEIGHBOUR_WEIGHT (0.25f)
/* noise_sample_row */

/* "three noise_lookup values combined" */

/* Smoothed noise value at a position (inferred: one axis of a noise field).
 * position_bits holds a float bit pattern; it is truncated to an int sample index.
 * Samples noise_lookup at index-1, index and index+1, then returns the centre
 * sample weighted by k_f_0_5 plus the two neighbours weighted by NOISE_NEIGHBOUR_WEIGHT.
 * noise_context is passed unchanged to noise_lookup (inferred: noise table or seed). */
double noise_sample_row(undefined4 noise_context,int position_bits)

{
  float noise_below;
  float noise_above;
  undefined4 sample_index;
  double centre_noise;

  /* lower neighbour */
  sample_index = ((int)(*(float *)&position_bits - 1.0f));
  noise_below = noise_lookup(noise_context,sample_index);
  /* upper neighbour */
  sample_index = ((int)(*(float *)&position_bits + 1.0f));
  noise_above = noise_lookup(noise_context,sample_index);
  /* centre sample */
  sample_index = ((int)(*(float *)&position_bits));
  centre_noise = (double)noise_lookup(noise_context,sample_index);
  return centre_noise * (double)k_f_0_5 + ((noise_below + noise_above) * NOISE_NEIGHBOUR_WEIGHT);
}


