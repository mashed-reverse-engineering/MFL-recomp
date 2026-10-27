#include "mfl_tier2.h"
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*006c5e20*/ extern undefined g_gfx_6c5e20;
/*006f1e20*/ extern undefined1 g_gfx_6f1e20;
#define EFFECT_POOL_BYTES (0x2c000)
#define EFFECT_BITMASK_BYTES (0x20)
/* effect_pool_clear */
/* "clears 0x2C000 bytes at 0x6C5E20 and the 0x20 byte bitmask at 0x6F1E20" */
/* Resets the effect pool: zeroes the whole pool (EFFECT_POOL_BYTES at g_gfx_6c5e20) and
   the bitmask (inferred: per-slot in-use flags) (EFFECT_BITMASK_BYTES at g_gfx_6f1e20). Takes no parameters, returns nothing. */
void effect_pool_clear(void)
{
  mem_zero(&g_gfx_6c5e20,EFFECT_POOL_BYTES);
  mem_zero(&g_gfx_6f1e20,EFFECT_BITMASK_BYTES);
  return;
}
