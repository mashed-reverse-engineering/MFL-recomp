#include "mfl_tier2.h"
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);

/* Capacity of the local atomic buffer; clump_get_atomics is given no size, so this is the limit (inferred). */
#define MAX_CLUMP_ATOMICS (256)

/*
 * Gathers the atomics of a clump and files each one into out_atomics at the slot
 * FUN_004c52b0 returns for it (slot index, 4-byte entries) (inferred).
 *   clump:          RpClump whose atomics are collected.
 *   out_atomics:    output array of atomic pointers, indexed by slot.
 *   unused_param_3: never read.
 *   param_4, param_5: passed unchanged to FUN_004c52b0; meaning unknown.
 * Returns the number of atomics found in the clump.
 */

int FUN_004c52f0(undefined4 clump,char *out_atomics,char *unused_param_3,undefined4 param_4,
                undefined4 param_5)

{
  undefined4 atomic;
  int atomic_count;
  int slot;
  int i;
  undefined4 atomics [MAX_CLUMP_ATOMICS];

  atomic_count = clump_get_atomics(clump,atomics);
  i = 0;
  if (0 < atomic_count) {
    /* For each atomic: ask FUN_004c52b0 for its slot and store it there. */
    do {
      atomic = atomics[i];
      slot = FUN_004c52b0(atomic,param_4,param_5);
      i = i + 1;
      *(undefined4 *)(out_atomics + slot * 4) = atomic;
    } while (i < atomic_count);
  }
  return atomic_count;
}


