#include "mfl_tier2.h"
/* gap_5c8220 */

/* no context */

struct SlotObject;
/* vtable entry of a pooled group object: release function and its argument */
typedef struct SlotVtable {
  void (*release)(struct SlotObject *obj, undefined4 arg);
  undefined4 release_arg;
} SlotVtable;

typedef struct SlotObject {
  char unknown_00[0x10];
  SlotVtable *vtbl;
} SlotObject;

/* slot pool: group count at +0x64, group masks at +0x6c, group objects at +0x70, slot -> group table at +0x78; mask width at +0x4c */
typedef struct SlotPool {
  char unknown_00[0x4c];
  undefined4 mask_bits;
  char unknown_50[0x14];
  uint group_count;
  char unknown_68[4];
  uint *group_masks;
  SlotObject **group_objects;
  char unknown_74[4];
  uint *slot_group;
} SlotPool;

/* handle of one slot: its pool and the slot index */
typedef struct SlotHandle {
  SlotPool *pool;
  int slot;
} SlotHandle;

/* Releases `handle` (pool, slot) from a slot pool: clears the slot bit in its group mask, frees the group object via its vtable entry
   once the masked bits are all clear, then advances the slot-group cursor with wraparound at the group count (+0x64). */
void gap_5c8220(char *unused_ctx,SlotHandle *handle)

{
  uint *groupOfSlot;
  int slot;
  SlotPool *pool;
  int group;
  SlotVtable *vtbl;
  uint curGroup;
  SlotObject *obj;

  slot = handle->slot;
  pool = handle->pool;
  group = pool->slot_group[slot];
  groupOfSlot = &pool->slot_group[slot];
  pool->group_masks[group] =
       pool->group_masks[group] & ~(1 << ((byte)slot & 0x1f));
  if ((pool->group_masks[*groupOfSlot] &
      (1 << ((byte)pool->mask_bits & 0x1f)) - 1U) == 0) {
    obj = pool->group_objects[*groupOfSlot];
    vtbl = obj->vtbl;
    vtbl->release(obj,vtbl->release_arg);
    pool->group_objects[*groupOfSlot] = 0;
  }
  curGroup = *groupOfSlot;
  *groupOfSlot = curGroup + 1;
  if (pool->group_count <= curGroup + 1) {
    *groupOfSlot = 0;
  }
  return;
}
