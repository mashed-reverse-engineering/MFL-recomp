#include "mfl_tier2.h"
/* gap_5bb400 */

/* no context */

/* Copies byte_count bytes from src to (*(obj+0x4c) + offset) (dword loop then byte tail, memcpy-like); returns obj (inferred). */
char *gap_5bb400(char *obj,int offset,undefined4 *src,uint byte_count)

{
  uint dword_count;
  undefined4 *dst;
  
  dst = (undefined4 *)(*(char **)(obj + 0x4c) + offset);
  for (dword_count = byte_count >> 2; dword_count != 0; dword_count = dword_count - 1) {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  }
  for (byte_count = byte_count & 3; byte_count != 0; byte_count = byte_count - 1) {
    *(undefined1 *)dst = *(undefined1 *)src;
    src = (undefined4 *)((char *)src + 1);
    dst = (undefined4 *)((char *)dst + 1);
  }
  return obj;
}


