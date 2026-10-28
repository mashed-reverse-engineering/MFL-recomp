#include "mfl_tier2.h"
/* gap_5d0000 */

/* no context */

/* Copies the 4 words pointed to by obj+0x10 into out_vec4 and returns 0, or E_FAIL (0x80004005) if that pointer is NULL (inferred). */
undefined4 __stdcall gap_5d0000(char *obj, undefined4 *out_vec4)

{
  undefined4 *src_vec4;
  
  src_vec4 = *(undefined4 **)(obj + 0x10);
  if (src_vec4 != (undefined4 *)0x0) {
    *out_vec4 = *src_vec4;
    out_vec4[1] = src_vec4[1];
    out_vec4[2] = src_vec4[2];
    out_vec4[3] = src_vec4[3];
    return 0;
  }
  return 0x80004005;
}


