#include "mfl_tier2.h"
#define SEVEN_BIT_WIDTH (7)
#define NINE_BIT_WIDTH (9)
#define SEVEN_BIT_MASK (0x7f)
#define NINE_BIT_MASK (0x1ff)
#define SEVEN_BIT_SCALE (127.0f)
#define NINE_BIT_SCALE (511.0f)
#define SEVEN_BIT_INV_SCALE (0.007874016f) /* 1/127 (inferred: undoes SEVEN_BIT_SCALE) */
/* course_cell_hash_pack */
/*
 * Packs two unit floats and two 7-bit values into one 32-bit course cell key.
 * Layout, MSB first: x_q9 (9 bits) | aux_a_q7 (7 bits) | y_q9 (9 bits) | aux_b_q7 (7 bits).
 * x_unit, y_unit: float values passed as raw bits; scaled by 511 and truncated to int.
 *   x_unit is not masked, so it is assumed to be in 0..1 (inferred).
 * aux_a, aux_b: integers, assumed 0..127 (inferred); rescaled through 1/127 and 127, then masked to 7 bits.
 * param_5..param_7 are unused.
 * Returns the packed key.
 * (was: "packs four float values into a 7/9/7/7 bit key"; aux_a and aux_b are ints, not floats)
 */
uint __cdecl course_cell_hash_pack(undefined4 x_unit,undefined4 y_unit,int aux_a,int aux_b,char *param_5, char *param_6,char *param_7)
{
  uint aux_a_q7;
  int x_q9;
  uint y_q9;
  uint aux_b_q7;

  /* quantize aux_a to 7 bits */
  aux_a_q7 = ((int)(((float)(uint)aux_a * SEVEN_BIT_INV_SCALE) * SEVEN_BIT_SCALE));
  /* quantize x_unit to 9 bits */
  x_q9 = ((int)(*(float *)&x_unit * NINE_BIT_SCALE));
  /* quantize y_unit to 9 bits */
  y_q9 = ((int)(*(float *)&y_unit * NINE_BIT_SCALE));
  /* quantize aux_b to 7 bits */
  aux_b_q7 = ((int)(((float)(uint)aux_b * SEVEN_BIT_INV_SCALE) * SEVEN_BIT_SCALE));
  /* pack: x_q9 | aux_a_q7 | y_q9 | aux_b_q7 */
  return aux_b_q7 & SEVEN_BIT_MASK | ((aux_a_q7 & SEVEN_BIT_MASK | x_q9 << SEVEN_BIT_WIDTH) << NINE_BIT_WIDTH | y_q9 & NINE_BIT_MASK) << SEVEN_BIT_WIDTH;
}
