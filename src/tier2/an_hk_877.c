#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dda20*/ extern float k_f_4_2949673e9;
/* hsv_phase_to_rgb */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/*
 * Maps a phase, value / range, onto four colour sectors in the R/G plane.
 * out[0] = R, out[1] = G; blue is not written.
 * value is in [0, range): sector = floor(4 * value / range). A sector >= 4
 * (value >= range) writes nothing. Negative values are read as unsigned (inferred).
 * (was: "(out, value, range): 6 segment phase to colour")
 */

void hsv_phase_to_rgb(float *out,int value,uint range)

{
  float value_f;
  double fraction;
  float range_f;

  /* int -> float; negative values get 2^32 added back (unsigned conversion) */
  value_f = (float)value;
  if (value < 0) {
    value_f = value_f + k_f_4_2949673e9;
  }
  range_f = (float)(int)range;
  if ((int)range < 0) {
    range_f = range_f + k_f_4_2949673e9;
  }
  /* sector index 0..3; range is overwritten with it (inferred from the reuse) */
  range = (uint)(value << 2) / range;
  fraction = (float)(int)range;
  if ((int)range < 0) {
    fraction = fraction + k_f_4_2949673e9;
  }
  /* position inside the sector, 0..1 (k_f_4 = 4.0 inferred from its name) */
  fraction = ((double)value_f * k_f_4) / range_f - fraction;
  /* each sector sets two channels; blue is left alone */
  switch(range) {
  case 0:
    /* R rises 0 -> 1, G = 0 */
    *out = (float)fraction;
    out[1] = 0.0;
    return;
  case 1:
    /* R = 1, G rises 0 -> 1 */
    out[1] = (float)fraction;
    *out = 1.0;
    return;
  case 2:
    /* G = 1, R falls 1 -> 0 */
    fraction = float_1_0 - fraction;
    out[1] = 1.0;
    *out = (float)fraction;
    return;
  case 3:
    /* R = 0, G falls 1 -> 0 */
    fraction = float_1_0 - fraction;
    *out = 0.0;
    out[1] = (float)fraction;
  }
  return;
}


