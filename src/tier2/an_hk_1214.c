#include "mfl_tier2.h"
/* gap_5c8220 */

/* no context */

/* Releases `handle` (pool, slot) from a slot pool: clears the slot bit in its group mask, frees the group object via its vtable entry
   once the masked bits are all clear, then advances the slot-group cursor with wraparound at the group count (+0x64). */
void gap_5c8220(char *unused_ctx,int *handle)

{
  uint *groupOfSlot;
  int slot;
  int pool;
  int group;
  undefined4 *vtbl;
  uint curGroup;
  
  slot = handle[1];
  pool = *handle;
  group = *(int *)(*(char **)(pool + 0x78) + slot * 4);
  groupOfSlot = (uint *)(*(char **)(pool + 0x78) + slot * 4);
  *(uint *)(*(char **)(pool + 0x6c) + group * 4) =
       *(uint *)(*(char **)(pool + 0x6c) + group * 4) & ~(1 << ((byte)slot & 0x1f));
  if ((*(uint *)(*(char **)(pool + 0x6c) + *groupOfSlot * 4) &
      (1 << ((byte)*(undefined4 *)(pool + 0x4c) & 0x1f)) - 1U) == 0) {
    slot = *(int *)(*groupOfSlot * 4 + *(char **)(pool + 0x70));
    vtbl = *(undefined4 **)(slot + 0x10);
    (*(code *)*vtbl)(slot,vtbl[1]);
    *(undefined4 *)(*(char **)(pool + 0x70) + *groupOfSlot * 4) = 0;
  }
  curGroup = *groupOfSlot;
  *groupOfSlot = curGroup + 1;
  if (*(uint *)(pool + 100) <= curGroup + 1) {
    *groupOfSlot = 0;
  }
  return;
}


