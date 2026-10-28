#include "mfl_tier2.h"
/* gap_5c6990 */

/* no context */

int gap_5c6990(byte *src,int byte_count,byte *dst)

{
  byte first_byte;
  int block_count;
  int half_block_countdown;
  
  block_count = (int)(byte_count + 0xf + (byte_count + 0xf >> 0x1f & 0xfU)) >> 4;
  byte_count = block_count;
  if (0 < block_count) {
    do {
      first_byte = *src;
      src = src + 2;
      half_block_countdown = 2;
      do {
        *dst = 0xc - (first_byte & 0xf) | first_byte & 0xf0;
        dst[1] = *src >> 4 | *src << 4;
        dst[2] = src[1] >> 4 | src[1] << 4;
        dst[3] = src[2] >> 4 | src[2] << 4;
        dst[4] = src[3] >> 4 | src[3] << 4;
        dst[5] = src[4] >> 4 | src[4] << 4;
        dst[6] = src[5] >> 4 | src[5] << 4;
        dst[7] = src[6] >> 4 | src[6] << 4;
        src = src + 7;
        dst = dst + 8;
        half_block_countdown = half_block_countdown + -1;
      } while (half_block_countdown != 0);
      byte_count = byte_count + -1;
    } while (byte_count != 0);
  }
  return block_count << 4;
}


