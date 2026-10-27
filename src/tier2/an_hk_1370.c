#include "mfl_tier2.h"
/* gap_5d2cc0 */

/* no context */

/* (inferred) copies byte_count bytes from buffer into the stream data area at stream+0x18 (dwords first, then the 0..3 trailing bytes); always returns 0 */
undefined4 __stdcall gap_5d2cc0(char *stream, uint byte_count, undefined4 *buffer)

{
  uint left;
  undefined4 *cursor;
  
  cursor = (undefined4 *)(stream + 0x18);
  for (left = byte_count >> 2; left != 0; left = left - 1) {
    *cursor = *buffer;
    buffer = buffer + 1;
    cursor = cursor + 1;
  }
  for (left = byte_count & 3; left != 0; left = left - 1) {
    *(undefined1 *)cursor = *(undefined1 *)buffer;
    buffer = (undefined4 *)((char *)buffer + 1);
    cursor = (undefined4 *)((char *)cursor + 1);
  }
  return 0;
}


