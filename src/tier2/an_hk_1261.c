#include "mfl_tier2.h"
/* gap_5cae30 */

/* no context */

/* Copy the 12-dword block at obj+0x40 into dst (inferred getter); returns obj. */
char *gap_5cae30(char *obj,char *unused_2,undefined4 *dst)

{
  int words_left;
  undefined4 *src;
  
  src = (undefined4 *)(obj + 0x40);
  for (words_left = 0xc; words_left != 0; words_left = words_left + -1) {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  }
  return obj;
}


