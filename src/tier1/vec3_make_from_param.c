#include "mfl_tier2.h"
#define FLOAT_1_0_BITS (0x3f800000) /* 1.0f */
#define FLOAT_0_01_BITS (0x3c23d70a) /* 0.01f */
/* vec3_make_from_param
 * Builds an output record from a source vector and a scalar. Parameters:
 *   src - pointer to a source vec3; only src[1] (offset 4) is read
 *   x   - scalar stored as out[0]
 *   out - destination record (undefined4 slots, indexed by dword)
 * Writes out[0] = x, out[1] = src[1], zeroes the other vector slots,
 * sets out[3] = 1.0f and out[5] = 0.01f, then clears/sets flags in the
 * block that out[0x18] points to (inferred: a sub-object pointer).
 * Returns nothing.
 */
/* "(src, x, out): out = {x, src+4, 0}" */
undefined4 * vec3_make_from_param(char *src,undefined4 x,undefined4 *out)
{
  undefined4 src_y;

  src_y = *(undefined4 *)(src + 4);
  *out = x;
  out[1] = src_y;
  out[2] = 0;
  out[4] = 0;
  out[6] = 0;
  out[7] = 0;
  out[8] = 0;
  out[9] = 0;
  out[10] = 0;
  out[3] = FLOAT_1_0_BITS;
  out[5] = FLOAT_0_01_BITS;
  *(undefined4 *)(out[0x18] + 0x3c) = 0;
  *(undefined4 *)(out[0x18] + 0x40) = 0;
  *(undefined4 *)(out[0x18] + 0x44) = 0;
  *(undefined4 *)(out[0x18] + 0x38) = 1;
  *(undefined4 *)(out[0x18] + 0x48) = 0;
  *(undefined4 *)(out[0x18] + 0x50) = 1;
  return out;
}
