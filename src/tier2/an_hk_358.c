#include "mfl_tier2.h"
static int weapon_oil_pick_slot_r(int reg_eax);
extern void weapon_oil_pick_slot(void);
__declspec(naked) int weapon_oil_pick_slot_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call weapon_oil_pick_slot
        ret
    }
}

/* weapon_oil_pick_slot_forward */

/* "calls weapon_oil_pick_slot" */

/* Forwards the weapon oil pick-slot request to weapon_oil_pick_slot, passing slot_index in eax.
 * slot_index (inferred): the slot value handed to weapon_oil_pick_slot. */
void weapon_oil_pick_slot_forward(undefined4 slot_index)

{
  weapon_oil_pick_slot_r(slot_index);
  return;
}


