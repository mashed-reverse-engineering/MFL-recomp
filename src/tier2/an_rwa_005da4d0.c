/* rwa_soft_wave_alloc_data_005da4d0 (0x005da4d0): software mixer wave: allocate the sample block */
#include "mfl_tier2.h"

/*005bf130*/ extern void *FUN_005bf130(int size, int align, int hint);      /* RWA aligned allocator */

/* Size = the wave's byte length (+0x34) plus 2 guard bytes for the interpolating mixer; memory hint 0x30806.
   Returns the wave, or NULL when the allocation failed. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_soft_wave_alloc_data_005da4d0(char *wave, int owner)
{
  void *data;

  *(int *)(wave + 0x50) = owner;
  data = FUN_005bf130(*(int *)(wave + 0x34) + 2, 1, 0x30806);
  *(void **)(wave + 0x4c) = data;
  return data != 0 ? wave : 0;
}
