#include "mfl_tier2.h"
/* gap_5cae10 */

/* no context */

/* Copies 12 dwords (48 bytes) from src into obj+0x40 and sets flag 0x10 at obj+0x28 (inferred). */
char *gap_5cae10(char *obj,char *unused_2,undefined4 *src)

{
  int count;
  undefined4 *dst;
  
  dst = (undefined4 *)(obj + 0x40);
  for (count = 0xc; count != 0; count = count + -1) {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  }
  *(uint *)(obj + 0x28) = *(uint *)(obj + 0x28) | 0x10;
  return obj;
}


