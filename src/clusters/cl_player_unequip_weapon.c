// cluster-entries: player_unequip_weapon
/* Installed cluster player_unequip_weapon (tools/cluster_install.py from output/cluster_pilot10/player_unequip_weapon.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* player_unequip_weapon */

/* esi = player weapon struct; calls descriptor+0x10 then zeroes +0xA8/+0xAC; NO null check on +0xA8
   (crashes if no weapon) */

void player_unequip_weapon_n(WeaponSlot *weapon_slot)

{
  (**(code **)(weapon_slot->descriptor + 0x10))(weapon_slot);
  weapon_slot->descriptor = 0;
  weapon_slot->state = 0;
  return;
}

/* entry thunk at the original address of player_unequip_weapon: register inputs (esi) -> natural C player_unequip_weapon_n; ret 0 */
__declspec(naked) void player_unequip_weapon(void)
{
    __asm {
        push esi
        call player_unequip_weapon_n
        add esp, 4
        ret
    }
}
