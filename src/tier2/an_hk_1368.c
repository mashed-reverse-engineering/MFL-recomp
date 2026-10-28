#include "mfl_tier2.h"
#define NO_RANGE_RESULT (0x80040251) /* error code returned when flag 2 is not set (inferred) */
/* gap_5d2c00 */

/* no context */

/* If obj flag 2 (+0x14) is set, copies two float pairs from obj+0x48 and obj+0x50 to out_a/out_b and returns 0, else returns an error code (inferred). */
undefined4 __stdcall gap_5d2c00(char *obj, undefined4 *out_a, undefined4 *out_b)

{
  if ((*(byte *)(obj + 0x14) & 2) != 0) {
    *out_a = *(undefined4 *)(obj + 0x48);
    out_a[1] = *(undefined4 *)(obj + 0x4c);
    *out_b = *(undefined4 *)(obj + 0x50);
    out_b[1] = *(undefined4 *)(obj + 0x54);
    return 0;
  }
  return NO_RANGE_RESULT;
}


