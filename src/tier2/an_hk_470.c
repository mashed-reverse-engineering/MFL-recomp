#include "mfl_tier2.h"
extern char PTR_weapon_machinegun_reset_all_0060dc44[];
/*0060de60*/ extern undefined4 weapon_descriptor_count;
/* stride between descriptors in the pointer table, in undefined ** slots (inferred: 0x40 bytes per descriptor) */
#define WEAPON_DESCRIPTOR_STRIDE (0x10)
/* weapons_reset_all_dispatch */

/* "calls the reset_all callback of every weapon descriptor" */

/* Calls the reset_all callback of every weapon descriptor (inferred: the first pointer of each
   descriptor in the table at PTR_weapon_machinegun_reset_all_0060dc44 is that callback).
   Takes no parameters and returns nothing. Does nothing when weapon_descriptor_count <= 0. */
void weapons_reset_all_dispatch(void)

{
  int descriptor_index;
  undefined **descriptor_entry;

  descriptor_index = 0;
  if (0 < (int)weapon_descriptor_count) {
    descriptor_entry = &PTR_weapon_machinegun_reset_all_0060dc44;
    /* walk the descriptor table, calling each reset_all callback in order */
    do {
      (*(code *)*descriptor_entry)();
      descriptor_index = descriptor_index + 1;
      descriptor_entry = descriptor_entry + WEAPON_DESCRIPTOR_STRIDE;
    } while (descriptor_index < (int)weapon_descriptor_count);
  }
  return;
}


