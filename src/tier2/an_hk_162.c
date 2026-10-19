#include "mfl_tier2.h"

#define POOL_ENTRY_SIZE (0x14) /* bytes per entry in the pool's entry array (pool + 4) */
#define ENTRY_FLAGS_CLEAR_08_MASK (0xfffffff7) /* clears bit 0x8 of an entry's first word (inferred: active flag) */

/*
 * Takes the next entry from a ring-buffer pool and clears its flag bit 0x8.
 *
 * pool is a byte descriptor (inferred layout):
 *   pool[0]  = base offset added to the cursor
 *   pool[1]  = cursor (head), advanced on each call
 *   pool[3]  = number of entries (capacity, inferred)
 *   pool + 4 = pointer to the entry array, POOL_ENTRY_SIZE bytes per entry
 *
 * Returns nothing; the entry is modified in place. If the cursor already
 * equals the capacity the pool is exhausted, entry stays NULL and the store
 * below faults (caller must check first, inferred).
 */
void FUN_005c1f80(byte *pool)

{
  byte head;
  byte count;
  uint *entry;
  uint slot;

  head = pool[1];
  count = pool[3];
  entry = (uint *)0x0;
  /* pool not exhausted: wrap (head + base) into [0, count) and advance the cursor */
  if (head != count) {
    slot = (uint)head + (uint)*pool;
    if (count <= slot) {
      slot = slot - count;
    }
    pool[1] = head + 1;
    entry = (uint *)(*(char **)(pool + 4) + slot * POOL_ENTRY_SIZE);
  }
  /* clear the flag bit on the entry's first word */
  *entry = *entry & ENTRY_FLAGS_CLEAR_08_MASK;
  return;
}


