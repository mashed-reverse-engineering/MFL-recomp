#include "mfl_tier2.h"
/*0048e650*/ extern int rwp_object_set_in_world();
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, int p1, int p2);
/*0056b220*/ extern void rwp_world_set_bit_60_64(int p0, int p1, int p2);
/*0056b300*/ extern void FUN_0056b300(undefined4 p0, undefined4 p1);
/*0056bc20*/ extern void FUN_0056bc20(undefined4 p0, undefined4 p1, undefined4 p2);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*006fab78*/ extern undefined4 g_rw_6fab78;
/* number of slots in the g_rw_6fab78 object table (inferred from the bounds check) */
#define MAX_OBJECTS (200)
/* rwp_object_activate */
/* "puts the object into the world and refreshes its state"
 * Activates the object held in slot object_index of the g_rw_6fab78 table: puts it into the world,
 * then looks up its owner object and refreshes that owner's world bits and state.
 * object_index: slot in g_rw_6fab78, valid range 0..MAX_OBJECTS-1 (inferred).
 * unused2, unused3: forwarded unchanged to FUN_0056bc20 (inferred).
 * Returns nothing. Does nothing if the index is out of range, the slot is empty, or there is no owner. */
void rwp_object_activate(int object_index, undefined4 unused2, undefined4 unused3)
{
  int object;

  /* range-check the index, then load the object from its slot; stop on an empty slot */
  if (((object_index >= 0) && (object_index < MAX_OBJECTS)) && (object = (&g_rw_6fab78)[object_index], object != 0)) {
    rwp_object_set_in_world(object_index,1);
    /* replace the slot object with its owner (inferred) */
    object = rwp_owner_object_get(object);
    if (object != 0) {
      /* refresh the owner: set world bits 0x60/0x64 and 0x5c, then run FUN_0056b300 (inferred) */
      rwp_world_set_bit_60_64(*(undefined4 *)(object + 0x24),object,1);
      rwp_world_set_bit_5c(*(undefined4 *)(object + 0x24),object,1);
      FUN_0056b300(*(undefined4 *)(object + 0x24),object);
      /* if the owner's sub-record at +0x10 has a non-zero field at +4, run FUN_0056bc20 on it (inferred) */
      if (*(int *)(*(char **)(object + 0x10) + 4) != 0) {
        FUN_0056bc20(*(int *)(*(char **)(object + 0x10) + 0x4), unused2, unused3);
        return;
      }
    }
  }
  return;
}
