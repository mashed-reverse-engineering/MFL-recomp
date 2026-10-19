#include "mfl_tier2.h"
/* FUN_005cdf40 */

/* Fills a 16-byte record at dst (inferred): the first word is value, the 16-bit
 * field at offset 4 is 0, the 16-bit field at offset 6 is 0x10, and the two
 * words at offsets 8 and 12 are fixed constants whose meaning is unknown.
 * value: word stored at dst[0]. dst: record to write. Returns nothing. */
void __cdecl FUN_005cdf40(undefined4 value,undefined4 *dst)

{
  *dst = value;
  *(undefined2 *)(dst + 1) = 0;
  *(undefined2 *)((char *)dst + 6) = 0x10;
  dst[2] = 0xaa000080;
  dst[3] = 0x719b3800;
  return;
}


