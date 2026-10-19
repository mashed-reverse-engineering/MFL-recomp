#include "mfl_tier2.h"
/*005543e0*/ extern int rw_second_field(int * p0);
/* rw_stream_chunk_copy */

/* "copies the payload of the current stream chunk into the destination buffer"
 * param_1: stream chunk, the payload starts 8 bytes behind it; its length is rw_second_field(param_1).
 */

void rw_stream_chunk_copy(undefined4 param_1,undefined4 *param_2)

{
  uint size;
  uint count;
  undefined4 *src;
  
  size = rw_second_field((int *)param_1);
  src = (undefined4 *)(param_1 + 8);
  for (count = size >> 2; count != 0; count = count - 1) {
    *param_2 = *src;
    src = src + 1;
    param_2 = param_2 + 1;
  }
  for (count = size & 3; count != 0; count = count - 1) {
    *(undefined1 *)param_2 = *(undefined1 *)src;
    src = (undefined4 *)((int)src + 1);
    param_2 = (undefined4 *)((int)param_2 + 1);
  }
  return;
}
