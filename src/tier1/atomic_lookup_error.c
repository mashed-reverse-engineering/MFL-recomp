#include "mfl_tier2.h"
/*004a8420*/ extern int debug_printf();
/* byte offsets of the three atomic slots in the table at lookup_state[1] (inferred) */
#define ATOMIC_SLOT0_OFFSET (4)
#define ATOMIC_SLOT1_OFFSET (8)
#define ATOMIC_SLOT2_OFFSET (0xc)
/* atomic_lookup_error */
/* "No such atomic as %d" */
/*
 * RpAtomicCallBack for RpClumpForAllAtomics (clump walk order).
 * atomic: the RpAtomic being visited.
 * lookup_state: int[2]; [0] = index of this atomic in the walk (count of atomics seen so far),
 *               [1] = pointer to the slot table (inferred).
 * Stores atomic in slot 0, 1 or 2 for indices 0, 1, 2; any other index is logged and dropped.
 * Always advances the counter and returns atomic, so the walk continues (inferred).
 */
undefined4 atomic_lookup_error(undefined4 atomic,int *lookup_state)
{
  switch (*lookup_state) {
  case 0:
    *(undefined4 *)(lookup_state[1] + ATOMIC_SLOT0_OFFSET) = atomic;
    break;
  case 1:
    *(undefined4 *)(lookup_state[1] + ATOMIC_SLOT1_OFFSET) = atomic;
    break;
  case 2:
    *(undefined4 *)(lookup_state[1] + ATOMIC_SLOT2_OFFSET) = atomic;
    break;
  default:
    debug_printf("No such atomic as %d\n",*lookup_state);
    break;
  }
  *lookup_state = *lookup_state + 1;
  return atomic;
}
