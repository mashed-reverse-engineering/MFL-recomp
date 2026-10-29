/* compile: /G5 */
#include "mfl_tier2.h"
/* Allocation size for n 40 byte records plus a power of two table (largest power of two not above n/2, doubled), 16 byte aligned. (Rwp library, proposed name rwp_batch_005793a0) */
int FUN_005793a0(unsigned n)
{
  unsigned p = 1;
  unsigned t;
  int a = ((n * 40 + 0xf) & ~0xf) + 0x10;
  for (t = n >> 1; t; t >>= 1) p += p;
  return a + ((p * 4 + 0xf) & ~0xf);
}
