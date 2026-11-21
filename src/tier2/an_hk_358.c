#include "mfl_tier2.h"
extern void weapon_oil_pick_slot_n(const int *matrix);

/* weapon_oil_pick_slot_forward */

/* "calls weapon_oil_pick_slot" */

/* Forwards the weapon oil pick-slot request to weapon_oil_pick_slot, passing slot_index in eax.
 * slot_index (inferred): the slot value handed to weapon_oil_pick_slot. */
void weapon_oil_pick_slot_forward(undefined4 slot_index)

{
  weapon_oil_pick_slot_n((const int *)slot_index);
  return;
}


