#include "mfl_tier2.h"
/*0058c8a0*/ extern char *rwp_owner_object_get(int p0);
/*006fd498*/ extern undefined4 g_rw_6fd498;
#define OWNER_SLOT_COUNT (0x10)
#define ROW_STRIDE (0x20)
#define ROW_VEC3_OFFSET (0x10)
/* Reads the velocity of the physics body owned by one player slot. */
/*
 * Reads the velocity of the physics body owned by one player slot.
 * out_vec: receives 3 floats (x, y, z) copied from the body's row.
 * owner_slot: index into g_rw_6fd498, must be below OWNER_SLOT_COUNT.
 * Returns 1 if the velocity was written, 0 if the slot has no body (inferred).
 */
undefined4 __cdecl physics_body_get_velocity(undefined4 *out_vec,uint owner_slot)
{
  undefined4 *src_vec;
  int *row_ref;
  char *owner_object;

  if (owner_slot < OWNER_SLOT_COUNT) {
    /* Look up the owner object, then follow its body reference chain (inferred) */
    owner_object = rwp_owner_object_get((&g_rw_6fd498)[owner_slot]);
    row_ref = *(int **)(*(char **)(owner_object + 0x10) + 4);
    if (row_ref != (int *)0x0) {
      /* Velocity vec3 sits ROW_VEC3_OFFSET into the row selected by row_ref[1] (inferred) */
      src_vec = (undefined4 *)(*(char **)(*(char **)(*row_ref + 0x10) + 8) + ROW_VEC3_OFFSET + row_ref[1] * ROW_STRIDE);
      *out_vec = *src_vec;
      out_vec[1] = src_vec[1];
      out_vec[2] = src_vec[2];
      return 1;
    }
  }
  return 0;
}
