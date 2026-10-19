#include "mfl_tier2.h"
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, int p1, int p2);
/*0056b220*/ extern void rwp_world_set_bit_60_64(int p0, int p1, int p2);
/*0056b300*/ extern void FUN_0056b300(undefined4 p0, undefined4 p1);
/*0056bc20*/ extern void FUN_0056bc20(undefined4 p0, undefined4 p1, undefined4 p2);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*006fab78*/ extern undefined4 g_rw_6fab78;

#define MAX_OBJECT_ID (200)
#define OWNER_BODY_OFFSET (0x24)

/* rwp_object_sync_and_reset
 *
 * Syncs the physics body of the object with the given id and applies a matrix to it.
 *   object_id:   index into g_rw_6fab78 (valid range 0..MAX_OBJECT_ID-1)
 *   matrix_arg:  passed through to FUN_0056bc20 as its second argument (inferred: matrix or flags)
 * Does nothing if the id is out of range, the slot is empty, or the owner has no body.
 */

/* "for object id <200 syncs the physics body and applies a matrix" */

void rwp_object_sync_and_reset(int object_id,undefined4 matrix_arg)

{
  int *matrix_target;
  char *owner;

  /* range-check the id, look up its owner object, and require a body pointer at OWNER_BODY_OFFSET */
  if ((((-1 < object_id) && (object_id < MAX_OBJECT_ID)) && ((&g_rw_6fab78)[object_id] != 0)) &&
     ((owner = rwp_owner_object_get((&g_rw_6fab78)[object_id]), owner != 0 && (*(int *)(owner + OWNER_BODY_OFFSET) != 0))))
  {
    /* sync the body: set the 0x60/0x64 and 0x5c flags, then run FUN_0056b300 (inferred: physics sync) */
    rwp_world_set_bit_60_64(*(int *)(owner + OWNER_BODY_OFFSET),owner,1);
    rwp_world_set_bit_5c(*(undefined4 *)(owner + OWNER_BODY_OFFSET),owner,1);
    FUN_0056b300(*(undefined4 *)(owner + OWNER_BODY_OFFSET),owner);
    matrix_target = *(int **)(*(char **)(owner + 0x10) + 4);
    /* apply the matrix only when the owner's sub-block has a target (inferred) */
    if (matrix_target != (int *)0x0) {
      FUN_0056bc20(matrix_target,matrix_arg,matrix_target[1] * 0x40 + 0x30 + **(int **)(*matrix_target + 0x10));
    }
  }
  return;
}


