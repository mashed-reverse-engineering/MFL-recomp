#include "mfl_tier2.h"
/* gap_4c4a80 */

/* no context */

uint gap_4c4a80(char *unused_1,undefined4 value_a,undefined4 *src,undefined4 value_b,
               int *sink)

{
  int buffer;
  int capacity;
  int *count_ptr;
  int tag;
  int index;
  int dword_offset;
  
  /* appends one 0x40-byte record built from src to sink->buffer[*count]: src[0..2] copied, src[6], three vectors through the pointers src[7..9], then value_a/value_b/tag; *count is incremented; returns src when the buffer is full (count == capacity), else 0 */
  buffer = *sink;
  capacity = sink[2];
  count_ptr = (int *)sink[1];
  tag = sink[3];
  index = *count_ptr;
  dword_offset = index * 0x10;
  if (buffer != 0) {
    *(undefined4 *)(buffer + index * 0x40) = *src;
    *(undefined4 *)(buffer + 4 + index * 0x40) = src[1];
    *(undefined4 *)(buffer + 4 + (dword_offset + 1) * 4) = src[2];
    *(undefined4 *)(buffer + 4 + (dword_offset + 2) * 4) = src[6];
    *(undefined4 *)(buffer + 4 + (dword_offset + 3) * 4) = *(undefined4 *)src[7];
    *(undefined4 *)(buffer + 4 + (dword_offset + 4) * 4) = *(undefined4 *)(src[7] + 4);
    *(undefined4 *)(buffer + 4 + (dword_offset + 5) * 4) = *(undefined4 *)(src[7] + 8);
    *(undefined4 *)(buffer + 4 + (dword_offset + 6) * 4) = *(undefined4 *)src[8];
    *(undefined4 *)(buffer + 4 + (dword_offset + 7) * 4) = *(undefined4 *)(src[8] + 4);
    *(undefined4 *)(buffer + 4 + (dword_offset + 8) * 4) = *(undefined4 *)(src[8] + 8);
    *(undefined4 *)(buffer + 4 + (dword_offset + 9) * 4) = *(undefined4 *)src[9];
    *(undefined4 *)(buffer + 4 + (dword_offset + 10) * 4) = *(undefined4 *)(src[9] + 4);
    *(undefined4 *)(buffer + 4 + (dword_offset + 0xb) * 4) = *(undefined4 *)(src[9] + 8);
    *(undefined4 *)(buffer + (dword_offset + 0xd) * 4) = value_a;
    *(undefined4 *)(buffer + (dword_offset + 0xe) * 4) = value_b;
    *(int *)(buffer + 4 + (dword_offset + 0xe) * 4) = tag;
  }
  buffer = *count_ptr;
  *count_ptr = buffer + 1;
  return (buffer + 1 == capacity) - 1 & (uint)src;
}


