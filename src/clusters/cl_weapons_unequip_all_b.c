// cluster-entries: weapons_unequip_all_b
/* Installed cluster weapons_unequip_all_b (tools/cluster_install.py from output/cluster_pilot_kb/weapons_unequip_all_b.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern char PTR_nop_stub_0060dc50[];
/*0060de60*/ extern undefined4 weapon_descriptor_count;
/*006aaef8*/ extern undefined4 g_weapons_6aaef8[0x1000];
/*008bed20*/ extern struct WeaponSlot player_weapon_slots[4];
/* weapons_unequip_all_b */

static int player_unequip_weapon_r(WeaponSlot * p0);
extern void player_unequip_weapon(void);
static int weapon_fx_state_clear_r(WeaponSlot * p0);
extern void weapon_fx_state_clear(void);
/* Unequip and clear fx state for all 4 players' weapon slots, zero g_weapons_6aaef8 entries, then call the per-descriptor callback of each weapon descriptor (inferred). */
void weapons_unequip_all_b(void)

{
  WeaponSlot *weapon_slot;
  int descriptor_index;
  undefined4 *weapon_state_ptr;
  undefined **descriptor_cb_ptr;
  
  weapon_state_ptr = g_weapons_6aaef8;
  weapon_slot = player_weapon_slots;
  do {
    if (weapon_slot->descriptor != 0) {
      player_unequip_weapon_r(weapon_slot);
    }
    weapon_fx_state_clear_r(weapon_slot);
    *weapon_state_ptr = 0;
    weapon_slot = weapon_slot + 1;
    weapon_state_ptr = weapon_state_ptr + 1;
  } while ((int)weapon_slot < (int)&player_weapon_slots + 0x2d0);
  descriptor_index = 0;
  if (0 < (int)weapon_descriptor_count) {
    descriptor_cb_ptr = &PTR_nop_stub_0060dc50;
    do {
      (*(code *)*descriptor_cb_ptr)();
      descriptor_index = descriptor_index + 1;
      descriptor_cb_ptr = descriptor_cb_ptr + 0x10;
    } while (descriptor_index < (int)weapon_descriptor_count);
  }
  return;
}

/* player_unequip_weapon_r: C -> original player_unequip_weapon (registers esi) */
__declspec(naked) int player_unequip_weapon_r(WeaponSlot * p0)
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
__declspec(naked) int weapon_fx_state_clear_r(WeaponSlot * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_state_clear
        pop esi
        ret
    }
}
