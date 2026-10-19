#include "mfl_tier2.h"
extern char PTR_nop_stub_0060dc48[];
/*0060de60*/ extern undefined4 weapon_descriptor_count;
/*008bed20*/ extern struct WeaponSlot player_weapon_slots[4];
static int weapon_fx_state_clear_r(int reg_esi);
extern void weapon_fx_state_clear(void);
extern char g_weapons_8beff0[];
/* pointer-sized slots per weapon descriptor in PTR_nop_stub_0060dc48 (inferred) */
#define WEAPON_DESCRIPTOR_STRIDE_SLOTS (0x10)
__declspec(naked) int weapon_fx_state_clear_r(int reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_state_clear
        pop esi
        ret
    }
}

/* weapons_race_start_dispatch */

/* "calls the race_start callback of every weapon descriptor" */

/*
 * Race-start hook for the weapon system: calls the race_start callback of every
 * weapon descriptor, then clears the per-player weapon state of every slot in
 * player_weapon_slots[4]. Takes no parameters and returns nothing.
 */
void weapons_race_start_dispatch(void)

{
  int descriptor_index;
  WeaponSlot *weapon_slot;
  undefined **callback_slot;

  /* walk the descriptor table, one race_start callback per descriptor */
  descriptor_index = 0;
  if (0 < (int)weapon_descriptor_count) {
    callback_slot = &PTR_nop_stub_0060dc48;
    do {
      (*(code *)*callback_slot)();
      descriptor_index = descriptor_index + 1;
      callback_slot = callback_slot + WEAPON_DESCRIPTOR_STRIDE_SLOTS;
    } while (descriptor_index < (int)weapon_descriptor_count);
  }
  /* clear each player weapon slot; g_weapons_8beff0 is the end of the array (inferred) */
  weapon_slot = player_weapon_slots;
  do {
    weapon_fx_state_clear_r(weapon_slot);
    weapon_slot = weapon_slot + 1;
  } while ((LONG_PTR)weapon_slot < ((LONG_PTR)&g_weapons_8beff0));
  return;
}


