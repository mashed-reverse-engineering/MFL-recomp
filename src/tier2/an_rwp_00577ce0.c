/* rwp_grid_00577ce0 (0x00577ce0): second workspace size in bytes for a solver configuration (every block 16 byte aligned) */
#include "mfl_tier2.h"

#define MFL_ALIGN16(x) (((x) + 15) & ~15u)

/* cfg[10] = row count n (taken as 4n words), cfg[4], cfg[5], cfg[6] + cfg[8] = counts of further arrays.  The blocks of the first sum
   are allocated twice. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
unsigned int rwp_grid_00577ce0(const unsigned int *cfg)
{
  unsigned int words = cfg[10] * 4;
  unsigned int pairs = cfg[8] + cfg[6];
  unsigned int twice;

  twice = MFL_ALIGN16(cfg[4] * 4) * 3 + MFL_ALIGN16(words / 4 * 16) + MFL_ALIGN16(words * 4) * 7 + MFL_ALIGN16(cfg[5] * 48)
          + MFL_ALIGN16(cfg[5] * 32) + MFL_ALIGN16(pairs * 4);
  return MFL_ALIGN16(twice * 2 + MFL_ALIGN16(words / 3 * 4) + MFL_ALIGN16(pairs * 64) + MFL_ALIGN16(words * 64) + MFL_ALIGN16(pairs * 8));
}
