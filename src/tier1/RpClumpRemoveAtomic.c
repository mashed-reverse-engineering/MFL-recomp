/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Unlinks an atomic from its clump and clears its clump pointer; returns the clump. */
void *RpClumpRemoveAtomic(void *c, void *a)
{
  FLD(FLD(a, char *, 0x44), int, 0) = FLD(a, int, 0x40);
  FLD(FLD(a, char *, 0x40), int, 4) = FLD(a, int, 0x44);
  FLD(a, int, 0x3c) = 0;
  return c;
}
