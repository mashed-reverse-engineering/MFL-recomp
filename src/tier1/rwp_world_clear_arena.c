#include "mfl_tier2.h"
#define ARENA_SLOT_SIZE (0x24)
#define ARENA_SLOT_DWORDS (9)
#define ARENA_SLOT_LINK_OFFSET (0x1c)
#define ARENA_EXTRA_SLOTS (2)
/* rwp_world_clear_arena */
/* "clears the working arena and rebuilds the per-body pointers" */
/* Clears the working arena of world and rebuilds its slot ring (inferred from the loops).
   world: owner whose arena is reset. Its arena buffer is at +0x14, the head slot at +0x18,
   the cursor at +0x1c, the tail at +0x20, and the slot count at +0x16c.
   Zeroes (count + ARENA_EXTRA_SLOTS) slots, links count slots through their +0x1c field
   (ARENA_SLOT_LINK_OFFSET) into a ring closed on the head, then clears +0x168, +0x170, +0x194
   and sets +0x198 to 1 (meaning inferred). Returns nothing. */
extern void *__cdecl memset(void *, int, unsigned int);
#pragma intrinsic(memset)
void rwp_world_clear_arena(char *world)
{
  int slot;

  *(undefined4 **)(world + 0x18) = *(undefined4 **)(world + 0x14);
  memset(*(void **)(world + 0x14), 0, (*(int *)(world + 0x16c) + ARENA_EXTRA_SLOTS) * ARENA_SLOT_DWORDS * 4);
  slot = *(int *)(world + 0x18);
  *(int *)(world + 0x1c) = slot;
  *(int *)(slot + ARENA_SLOT_LINK_OFFSET) = slot + ARENA_SLOT_SIZE;
  slot = 0;
  if (0 < *(int *)(world + 0x16c)) {
    do {
      *(int *)(*(char **)(world + 0x1c) + ARENA_SLOT_LINK_OFFSET) = *(int *)(world + 0x1c) + ARENA_SLOT_SIZE;
      *(undefined4 *)(world + 0x1c) = *(undefined4 *)(*(char **)(world + 0x1c) + ARENA_SLOT_LINK_OFFSET);
      slot = slot + 1;
    } while (slot < *(int *)(world + 0x16c));
  }
  *(int *)(world + 0x20) = *(int *)(world + 0x1c);
  *(undefined4 *)(*(char **)(world + 0x1c) + ARENA_SLOT_LINK_OFFSET) = *(undefined4 *)(world + 0x18);
  *(undefined4 *)(world + 0x168) = 0;
  *(undefined4 *)(world + 0x170) = 0;
  *(undefined4 *)(world + 0x194) = 0;
  *(undefined4 *)(world + 0x198) = 1;
  *(undefined4 *)(world + 0x1c) = *(undefined4 *)(world + 0x18);
  return;
}
