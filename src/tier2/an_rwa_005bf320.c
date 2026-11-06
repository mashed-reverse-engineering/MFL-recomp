/* rwa_mem_reverse_005bf320 (0x005bf320): memory: reverse the byte order of a block in place */
#include "mfl_tier2.h"

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_mem_reverse_005bf320(unsigned char *mem, RwUInt32 bytes)
{
  unsigned char *end = mem + bytes - 1, t;
  RwUInt32 i;

  for (i = 0; i < bytes >> 1; i++, end--) {
    t = mem[i];
    mem[i] = *end;
    *end = t;
  }
}
