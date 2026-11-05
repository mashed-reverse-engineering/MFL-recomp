/* rwp_grid_00577970 (0x00577970): workspace size in bytes for a solver configuration (every block 16 byte aligned) */
#include "mfl_tier2.h"

#define MFL_ALIGN16(x) (((x) + 15) & ~15u)

/* cfg[10] = row count n (taken as 4n words, n per lane block), cfg[5] / cfg[4] = word counts of two further arrays. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
unsigned int rwp_grid_00577970(const unsigned int *cfg)
{
  unsigned int words = cfg[10] * 4;
  unsigned int n = words / 4;

  return MFL_ALIGN16(MFL_ALIGN16(words / 3 * 64) * 2 + MFL_ALIGN16(cfg[5] * 4) * 4 + MFL_ALIGN16(n * 8) * 4 + MFL_ALIGN16(n * 16) * 5
                     + MFL_ALIGN16(cfg[4] * 4) + MFL_ALIGN16(n * 192));
}
