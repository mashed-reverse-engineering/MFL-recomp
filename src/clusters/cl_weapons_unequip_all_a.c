// cluster-entries: weapons_unequip_all_a
/* Installed cluster weapons_unequip_all_a (tools/cluster_install.py from output/cluster_pilot_zx/weapons_unequip_all_a.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
extern char PTR_nop_stub_0060dc4c[];
/*0060de60*/ extern undefined4 weapon_descriptor_count;
/*008bed20*/ extern struct WeaponSlot player_weapon_slots[4];

extern char g_weapons_8beff0[];

/* Size of one PTR_nop_stub table entry, in undefined * elements (inferred: 16 pointers = 0x40 bytes per weapon descriptor). */
#define WEAPON_CALLBACK_STRIDE (0x10)


/* weapons_unequip_all_a
 * Unequips every weapon currently held in the player weapon slots, then runs each
 * weapon descriptor's callback from the PTR_nop_stub table (inferred: per-weapon reset hook).
 * Takes no parameters and returns nothing.
 */

static int player_unequip_weapon_r(int p0);
extern void player_unequip_weapon(void);
static int weapon_fx_state_clear_r(int p0);
extern void weapon_fx_state_clear(void);
void weapons_unequip_all_a(void)

{
  WeaponSlot *weapon_slot;
  int callback_index;
  undefined **callback_slot;

  /* Walk the player weapon slots up to g_weapons_8beff0 (inferred: end of the array); unequip any slot that holds a descriptor. */
  weapon_slot = player_weapon_slots;
  do {
    if (weapon_slot->descriptor != 0) {
      player_unequip_weapon_r(weapon_slot);
      weapon_fx_state_clear_r(weapon_slot);
    }
    weapon_slot = weapon_slot + 1;
  } while ((LONG_PTR)weapon_slot < ((LONG_PTR)&g_weapons_8beff0));
  /* Call the callback of every weapon descriptor (weapon_descriptor_count entries). */
  callback_index = 0;
  if (0 < (int)weapon_descriptor_count) {
    callback_slot = &PTR_nop_stub_0060dc4c;
    do {
      (*(code *)*callback_slot)();
      callback_index = callback_index + 1;
      callback_slot = callback_slot + WEAPON_CALLBACK_STRIDE;
    } while (callback_index < (int)weapon_descriptor_count);
  }
  return;
}

/* player_unequip_weapon_r: C -> original player_unequip_weapon (registers esi) */
__declspec(naked) int player_unequip_weapon_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call player_unequip_weapon
        pop esi
        ret
    }
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
