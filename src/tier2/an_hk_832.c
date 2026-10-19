#include "mfl_tier2.h"
/*0056bc20*/ extern void FUN_0056bc20(undefined4 p0, undefined4 p1, undefined4 p2);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*006fd498*/ extern undefined4 g_rw_6fd498;
/* car_apply_rwp_impulse */

/* Applies an impulse to a car's RWP (rigid body) physics object (inferred from the callee names).
 *   car_index: index into g_rw_6fd498, the per-car RWP handle table.
 *   impulse, position: passed through unchanged to FUN_0056bc20 (inferred: impulse vector and
 *     application point; this function never reads them).
 * Does nothing unless the owner's body sub-object has a non-zero word at +4.
 * Returns nothing.
 * (was: "(car, impulse vector, position): applies an impulse to the car's RWP body") */

void car_apply_rwp_impulse(int car_index, undefined4 impulse, undefined4 position)

{
  int rwp_owner;

  rwp_owner = rwp_owner_object_get((&g_rw_6fd498)[car_index]);
  /* Apply only when the owner's body pointer (+0x10) has a non-zero word at +4 (inferred: body is valid). */
  if (*(int *)(*(char **)(rwp_owner + 0x10) + 4) != 0) {
    FUN_0056bc20(*(int *)(*(char **)(rwp_owner + 0x10) + 0x4), impulse, position);
    return;
  }
  return;
}


