#include "mfl_tier2.h"
#define POOL_SLOT_STRIDE (0x14)      /* bytes per slot in the pool's slot array */
#define SLOT_INDEX_NOT_FOUND (0xffff) /* fallback index when no free flag is seen (inferred) */
/* pool_alloc_slot */
/* "allocates the first free slot (stride 0x14) of a pool" */
/*
 * Allocates one slot from a pool and appends it to the pool's list of used slots.
 * Returns the new slot, or NULL if the pool is full.
 *
 * pool: pool header (inferred layout, ushort indices):
 *   pool[0]     used slot count
 *   pool[1]     slot capacity
 *   pool+2      list head (oldest used slot)
 *   pool+4      list tail (newest used slot)
 *   pool+6      slot array base (POOL_SLOT_STRIDE bytes per slot)
 *   pool+8      per-slot in-use flags (one int per slot, 1 = in use)
 * value: stored in slot word 0 (inferred: caller's payload)
 * Slot words (inferred): [0] value, [1] prev, [2] next, [3] slot index, [4] owner pool.
 * unused_3..unused_9: unused by this function.
 */
undefined4 * __cdecl pool_alloc_slot(ushort *pool,undefined4 value,char *unused_3,char *unused_4, char *unused_5,char *unused_6,char *unused_7,char *unused_8, char *unused_9)
{
  undefined4 *slot;
  ushort slot_index;

  slot_index = pool[1];
  if (*pool >= slot_index) {
    return (undefined4 *)0x0;
  }
  /* scan down from the capacity for a slot whose in-use flag is 0 (inferred) */
  if (slot_index != 0) {
    do {
      slot_index = slot_index - 1;
      if (*(int *)(*(char **)(pool + 8) + (uint)slot_index * 4) == 0) goto LAB_00497639;
    } while (slot_index != 0);
  }
  slot_index = slot_index + SLOT_INDEX_NOT_FOUND;
LAB_00497639:
  slot = (undefined4 *)(*(char **)(pool + 6) + (uint)slot_index * POOL_SLOT_STRIDE);
  *(undefined4 *)(*(char **)(pool + 8) + (uint)slot_index * 4) = 1;
  *(ushort *)(slot + 3) = slot_index;
  *pool = *pool + 1;
  *slot = value;
  /* list is empty: the new slot becomes both head and tail */
  if (*(int *)(pool + 2) == 0) {
    *(undefined4 **)(pool + 2) = slot;
    slot[1] = 0;
    *(undefined4 *)(*(char **)(pool + 2) + 8) = 0;
    slot[4] = pool;
    return slot;
  }
  /* list has one slot (tail unset): append it as the tail after the head (inferred) */
  if (*(int *)(pool + 4) == 0) {
    *(undefined4 **)(pool + 4) = slot;
    slot[2] = 0;
    *(undefined4 *)(*(char **)(pool + 4) + 4) = *(undefined4 *)(pool + 2);
    *(undefined4 *)(*(char **)(pool + 2) + 8) = *(undefined4 *)(pool + 4);
    slot[4] = pool;
    return slot;
  }
  /* general case: link the new slot after the current tail */
  slot[1] = *(int *)(pool + 4);
  *(undefined4 **)(*(char **)(pool + 4) + 8) = slot;
  *(undefined4 **)(pool + 4) = slot;
  slot[2] = 0;
  slot[4] = pool;
  return slot;
}
