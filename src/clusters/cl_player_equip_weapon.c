// cluster-entries: player_equip_weapon
/* Installed cluster player_equip_weapon (tools/cluster_install.py from output/cluster_pilot_zx/player_equip_weapon.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */

#define PLAYER_EQUIP_DESCRIPTOR_OFFSET (0xa8)
#define PLAYER_EQUIP_RESULT_OFFSET (0xac)
#define DESCRIPTOR_EQUIP_FN_OFFSET (4)


/* player_equip_weapon */

/* cdecl (player weapon struct*; weapon type id); stores descriptor at +0xA8 and descriptor+4 result
   at +0xAC; NO null check on the descriptor (crashes for unknown id) */

/* Equips a weapon on a player: looks up the weapon's descriptor by id, stores it in the player,
   then calls the descriptor's equip function (inferred from the name) with the player and
   stores its return value in the player. Parameters: player = player weapon struct*,
   weapon_id = weapon type id (WeaponId). Returns nothing. */

static int * weapon_find_descriptor_r(int p0);
extern void weapon_find_descriptor(void);
void player_equip_weapon(int player,undefined4 weapon_id)

{
  int descriptor;
  undefined4 equip_result;

  descriptor = (int)(weapon_find_descriptor_r(weapon_id));
  *(int *)(player + PLAYER_EQUIP_DESCRIPTOR_OFFSET) = descriptor;
  equip_result = (**(int (**)())(descriptor + DESCRIPTOR_EQUIP_FN_OFFSET))(player);
  *(undefined4 *)(player + PLAYER_EQUIP_RESULT_OFFSET) = equip_result;
  return;
}

/* weapon_find_descriptor_r: C -> original weapon_find_descriptor (registers esi) */
__declspec(naked) int * weapon_find_descriptor_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_find_descriptor
        pop esi
        ret
    }
}
