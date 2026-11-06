/* rwa_mem_endian_swap_005bf360 (0x005bf360): memory (RwaMemEndianSwap): byte swap of 2 or 4 byte units in place */
#include "mfl_tier2.h"

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_mem_endian_swap_005bf360(void *mem, RwUInt32 bytes, int width)
{
  RwUInt32 n, v;

  if (width == 2) {
    unsigned char *p = (unsigned char *)mem;
    for (n = bytes >> 1; n != 0; n--, p += 2) {
      v = p[0] << 8 | p[1];
      *(unsigned short *)p = (unsigned short)v;
    }
  } else if (width == 4) {
    RwUInt32 *p = (RwUInt32 *)mem;
    for (n = bytes >> 2; n != 0; n--, p++) {
      v = *p;
      *p = v << 24 | (v & 0xff00) << 8 | (v >> 8 & 0xff00) | v >> 24;
    }
  }
}
