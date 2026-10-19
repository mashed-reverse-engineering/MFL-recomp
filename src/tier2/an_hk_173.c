#include "mfl_tier2.h"
/*0064aa00*/ extern undefined g_data_64aa00;
/*0064aa20*/ extern undefined g_data_64aa20;
#define MOVE_NEGATIVE (8)
#define MOVE_FULL (4)
#define MOVE_HALF (2)
#define MOVE_QUARTER (1)
#define POS_MIN (-0x8000)
#define POS_MAX (0x7fff)
#define SPEED_INDEX_MAX (0x58)
/* FUN_005c7150
 *
 * Moves one axis position toward a target and advances its speed index (inferred).
 *   target_pos   - target position, 16-bit signed
 *   position     - current position, updated in place and clamped to [POS_MIN, POS_MAX]
 *   speed_index  - index into the step table g_data_64aa20 (step size); updated in place
 *                  by the per-move delta from g_data_64aa00 and clamped to [0, SPEED_INDEX_MAX]
 * Returns nothing.
 */

void FUN_005c7150(short target_pos,int *position,int *speed_index)

{
  byte move_code;
  int step;
  int distance;
  int target;

  /* step size for the current speed index */
  step = (int)*(short *)((char *)&g_data_64aa20 + *speed_index * 2);
  target = (int)target_pos;
  distance = target - *position;
  /* move_code bits: NEGATIVE = moving down, FULL/HALF/QUARTER = sub-steps taken (inferred) */
  move_code = 0;
  if (distance < 0) {
    move_code = MOVE_NEGATIVE;
    distance = -distance;
  }
  /* take the full, half and quarter step while the remaining distance covers it */
  if (step <= distance) {
    move_code = move_code | MOVE_FULL;
    distance = distance - step;
  }
  if (step >> 1 <= distance) {
    move_code = move_code | MOVE_HALF;
    distance = distance - (step >> 1);
  }
  if (step >> 2 <= distance) {
    move_code = move_code | MOVE_QUARTER;
    distance = distance - (step >> 2);
  }
  /* new position from the step taken and the leftover distance (inferred) */
  if ((move_code & MOVE_NEGATIVE) == 0) {
    *position = ((step >> 3) - distance) + target;
  }
  else {
    *position = (target - (step >> 3)) + distance;
  }
  /* clamp position to the signed 16-bit range */
  if (*position < 0x8000) {
    if (*position < -0x8000) {
      *position = POS_MIN;
    }
  }
  else {
    *position = POS_MAX;
  }
  /* advance speed index by the delta for this move code; step is reused as the new index */
  step = *speed_index + (int)*(short *)((char *)&g_data_64aa00 + (uint)move_code * 2);
  *speed_index = step;
  /* clamp speed index to [0, SPEED_INDEX_MAX] */
  if (step < 0) {
    *speed_index = 0;
    return;
  }
  if (SPEED_INDEX_MAX < step) {
    *speed_index = SPEED_INDEX_MAX;
  }
  return;
}


