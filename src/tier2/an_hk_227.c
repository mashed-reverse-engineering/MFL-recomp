#include "mfl_tier2.h"
/*004c5440*/ extern int __cdecl FUN_004c5440(int a1, int a2, int a3);
#define OBJECT_SLOT_COUNT (0x80)
#define SLOT_CLUMP_ARRAY_OFFSET (0xc110)
#define SLOT_ACTIVE_OFFSET (0x2d4)
#define ATOMIC_FLAG_0X40 (0x40)
/* course_atomics_set_flag_0x40 */

/* "for each of the 128 object slots sets flag 0x40 on its clump atomics" */

/* Sets ATOMIC_FLAG_0X40 on the clump of every active object slot (128 slots).
 * world: base pointer; the clump array is at +SLOT_CLUMP_ARRAY_OFFSET (inferred).
 * slot_table: base pointer; the per-slot active flags are at +SLOT_ACTIVE_OFFSET, stride 5 ints (inferred).
 * Returns nothing. */
void course_atomics_set_flag_0x40(char *world,char *slot_table)

{
  int slots_left;
  undefined4 *slot_clump;
  int *slot_active;

  slot_clump = (undefined4 *)(world + SLOT_CLUMP_ARRAY_OFFSET);
  slot_active = (int *)(slot_table + SLOT_ACTIVE_OFFSET);
  slots_left = OBJECT_SLOT_COUNT;
  /* walk all slots; flag only those whose active flag is non-zero */
  do {
    if (*slot_active != 0) {
      FUN_004c5440(*slot_clump,ATOMIC_FLAG_0X40,1);
    }
    slot_clump = slot_clump + 1;
    slot_active = slot_active + 5;
    slots_left = slots_left + -1;
  } while (slots_left != 0);
  return;
}


