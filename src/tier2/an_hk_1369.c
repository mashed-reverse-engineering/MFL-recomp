#include "mfl_tier2.h"
/* gap_5d2c90 */

/* no context */

/* (inferred) copies byte_count bytes from the stream data area at stream+0x18 into buffer (dwords first, then the 0..3 trailing bytes); always returns 0 */
undefined4 __stdcall gap_5d2c90(char *stream, uint byte_count, undefined4 *buffer)

{
  uint left;
  undefined4 *cursor;
  
  cursor = (undefined4 *)(stream + 0x18);
  for (left = byte_count >> 2; left != 0; left = left - 1) {
    *buffer = *cursor;
    cursor = cursor + 1;
    buffer = buffer + 1;
  }
  for (left = byte_count & 3; left != 0; left = left - 1) {
    *(undefined1 *)buffer = *(undefined1 *)cursor;
    cursor = (undefined4 *)((char *)cursor + 1);
    buffer = (undefined4 *)((char *)buffer + 1);
  }
  return 0;
}


