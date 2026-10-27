// cluster-entries: weapons_race_start_dispatch
/* Installed cluster weapons_race_start_dispatch (tools/cluster_install.py from output/cluster_pilot_zx/weapons_race_start_dispatch.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
extern char PTR_nop_stub_0060dc48[];
/*0060de60*/ extern undefined4 weapon_descriptor_count;
/*008bed20*/ extern struct WeaponSlot player_weapon_slots[4];
extern char g_weapons_8beff0[];
/* pointer-sized slots per weapon descriptor in PTR_nop_stub_0060dc48 (inferred) */
#define WEAPON_DESCRIPTOR_STRIDE_SLOTS (0x10)

/* weapons_race_start_dispatch */

/* "calls the race_start callback of every weapon descriptor" */

/*
 * Race-start hook for the weapon system: calls the race_start callback of every
 * weapon descriptor, then clears the per-player weapon state of every slot in
 * player_weapon_slots[4]. Takes no parameters and returns nothing.
 */
static int weapon_fx_state_clear_r(int p0);
extern void weapon_fx_state_clear(void);
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

/* weapon_fx_state_clear_r: C -> original weapon_fx_state_clear (registers esi) */
__declspec(naked) int weapon_fx_state_clear_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_state_clear
        pop esi
        ret
    }
}
