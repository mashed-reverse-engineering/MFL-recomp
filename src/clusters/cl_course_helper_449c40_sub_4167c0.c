// cluster-entries: course_helper_449c40_sub_4167c0
/* Installed cluster course_helper_449c40_sub_4167c0 (tools/cluster_install.py from output/cluster_pilot_md/course_helper_449c40_sub_4167c0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00416760*/ extern float __cdecl noise_value(int octave, float x);
/*005dd340*/ extern float k_f_0_5;
/* course_helper_449c40_sub_4167c0 */

/* "three octave fractal noise of x + 1: sum of noise_value(octave, (x + 1) * 2^octave) / 2^octave, halved" */
float course_helper_449c40_sub_4167c0(float x)
{
  float shifted;
  float frequency;
  float amplitude;
  float sum;
  int octave;

  shifted = x + 1.0f;
  frequency = 1.0f;
  amplitude = 1.0f;
  sum = 0.0f;
  for (octave = 0; octave < 3; octave = octave + 1) {
    sum = noise_value(octave, shifted * frequency) * amplitude + sum;
    frequency = frequency + frequency;
    amplitude = amplitude * k_f_0_5;
  }
  return sum * k_f_0_5;
}

