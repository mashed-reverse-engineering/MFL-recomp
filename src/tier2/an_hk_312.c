#include "mfl_tier2.h"
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*006fab78*/ extern undefined4 g_rw_6fab78;

/* number of entries in the g_rw_6fab78 object table (inferred from the bounds check) */
#define RW_OBJECT_TABLE_SIZE (200)

/* rwp_object_get_matrix */

/* "(index): matrix pointer of the physics object" */

/*
 * Returns the matrix (undefined4 value at offset 8 of the block pointed to by
 * owner+0x10) of the physics object at object_index, or 0 if object_index is
 * out of range. The owner is looked up through the rw object table.
 * (inferred: the owner object is the rw object that owns the physics object)
 */
undefined4 rwp_object_get_matrix(int object_index)

{
  int owner_object;

  if ((-1 < object_index) && (object_index < RW_OBJECT_TABLE_SIZE)) {
    owner_object = rwp_owner_object_get((&g_rw_6fab78)[object_index]);
    return *(undefined4 *)(*(char **)(owner_object + 0x10) + 8);
  }
  return 0;
}


