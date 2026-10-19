#include "mfl_tier2.h"
/*0064aa00*/ extern undefined g_data_64aa00;
/*0064aa20*/ extern undefined g_data_64aa20;
#define SHORT_MIN (-0x8000)
#define SHORT_MAX (0x7fff)
#define LEVEL_MAX (0x58)
#define STEP_INDEX_MASK (7)
#define SIGN_BIT (8)
/* FUN_005c7200 */

/* Applies one packed step code to an accumulator and a level (inferred: a per-step
 * adjustment, e.g. a steering or trim value, with a 0..LEVEL_MAX level).
 *   packed_index: low 3 bits pick the step size (odd multiplier 1..15), bit 3 (SIGN_BIT)
 *                 negates the step, the whole value indexes g_data_64aa00 for the level change.
 *   accumulator:  int16 running value, clamped to [SHORT_MIN, SHORT_MAX].
 *   level:        current level, updated in place and clamped to [0, LEVEL_MAX].
 * Returns nothing; both outputs are written through the pointers. */
void FUN_005c7200(uint packed_index,int *accumulator,int *level)

{
  uint delta;
  int new_value;

  /* step = table entry g_data_64aa20[level] * odd multiplier from packed_index, divided by 8 */
  delta = (int)*(short *)((char *)&g_data_64aa20 + *level * 2) * ((packed_index & STEP_INDEX_MASK) * 2 + 1) >> 3;
  /* sign bit set: subtract the step instead of adding it */
  if ((packed_index & SIGN_BIT) != 0) {
    delta = -delta;
  }
  /* add the step to the accumulator and clamp it to the int16 range */
  new_value = *accumulator + delta;
  *accumulator = new_value;
  if (new_value < SHORT_MIN) {
    *accumulator = SHORT_MIN;
  }
  else if (SHORT_MAX < new_value) {
    *accumulator = SHORT_MAX;
  }
  /* move the level by the table entry g_data_64aa00[packed_index]; clamp to [0, LEVEL_MAX] */
  new_value = *level + (int)*(short *)((char *)&g_data_64aa00 + packed_index * 2);
  *level = new_value;
  if (-1 < new_value) {
    if (LEVEL_MAX < new_value) {
      *level = LEVEL_MAX;
    }
    return;
  }
  *level = 0;
  return;
}


