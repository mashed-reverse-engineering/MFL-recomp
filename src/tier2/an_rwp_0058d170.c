/* rwp_ptrset_insert (0x0058d170): insert a pointer into an open addressing set */
#include "mfl_tier2.h"

/* table[1..n] is a set of pointers with linear probing (0 = free slot); the probe starts at slot start + 1 and wraps from n to 1.
   Stores item in the first free slot unless it is already there.  Returns the slot of item. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwp_ptrset_insert(void *item, void **table, int n, int start)
{
  int i = start + 1;
  void *p = table[i];

  while (p) {
    if (p == item) return i;
    i = (i >= n ? 0 : i) + 1;
    p = table[i];
  }
  table[i] = item;
  return i;
}
