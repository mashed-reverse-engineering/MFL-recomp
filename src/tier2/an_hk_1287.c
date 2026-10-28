#include "mfl_tier2.h"
/* gap_5ccb30 */

/* no context */

/* Copy a 12-dword block from src into obj+0x44 and set flag 0x80 in obj+0x78 (inferred setter); returns obj. */
char *gap_5ccb30(char *obj,char *unused_2,undefined4 *src)

{
  int words_left;
  undefined4 *dst;
  
  dst = (undefined4 *)(obj + 0x44);
  for (words_left = 0xc; words_left != 0; words_left = words_left + -1) {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  }
  *(uint *)(obj + 0x78) = *(uint *)(obj + 0x78) | 0x80;
  return obj;
}


