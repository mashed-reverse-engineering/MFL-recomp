#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
/* FUN_0056f080 */


/*
 * Returns 1 if the box is at most 1.0 wide on every axis, else 0 (inferred).
 * box[0..2] = low corner (x, y, z), box[4..6] = high corner; box[3] is unused.
 * Per axis: high <= low + 1.0 (inferred meaning: small/point-like box test).
 */
undefined4 FUN_0056f080(float *box)

{
  /* all three axes must satisfy the 1.0 margin */
  if (((box[4] <= *box + float_1_0) && (box[5] <= box[1] + float_1_0)) &&
     (box[6] <= box[2] + float_1_0)) {
    return 1;
  }
  return 0;
}


