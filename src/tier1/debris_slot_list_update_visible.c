#include "mfl_tier2.h"
/*00424ab0*/ extern int rwp_world_current_deactivate();

#define DEBRIS_SLOT_COUNT 10
#define DEBRIS_SLOT_WORDS 6  /* 0x18 bytes per debris slot */
#define DEBRIS_SLOTS_OFFSET 0x10 /* first slot inside the debris list struct */


/* debris_slot_list_update_visible */

/* "for the 10 slots (stride 0x18) with a clump calls game_fn_424ab0" */

/* For each debris slot that holds a clump with a non-zero field at +0x24, deactivates it; then clears
   the two list fields at +0xf0 and +0xf4 (names unknown). */
void __cdecl debris_slot_list_update_visible(char *debris_list)
{
  int *slot;
  int slots_left;
  
  slot = (int *)(debris_list + DEBRIS_SLOTS_OFFSET);
  slots_left = DEBRIS_SLOT_COUNT;
  do {
    /* slot[0] = clump pointer; +0x24 of that clump is checked (name unknown) */
    if ((*slot != 0) && (*(int *)(*slot + 0x24) != 0)) {
      rwp_world_current_deactivate();
    }
    slot = slot + DEBRIS_SLOT_WORDS;
    slots_left = slots_left + -1;
  } while (slots_left != 0);
  /* list fields at +0xf4 and +0xf0 (names unknown) */
  *(undefined4 *)(debris_list + 0xf4) = 0;
  *(undefined4 *)(debris_list + 0xf0) = 0;
  return;
}
