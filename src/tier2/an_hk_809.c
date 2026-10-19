#include "mfl_tier2.h"
static int * weapon_find_descriptor_r(int reg_esi);
extern void weapon_find_descriptor(void);

#define PLAYER_EQUIP_DESCRIPTOR_OFFSET (0xa8)
#define PLAYER_EQUIP_RESULT_OFFSET (0xac)
#define DESCRIPTOR_EQUIP_FN_OFFSET (4)

__declspec(naked) int * weapon_find_descriptor_r(int reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_find_descriptor
        pop esi
        ret
    }
}

/* player_equip_weapon */

/* cdecl (player weapon struct*; weapon type id); stores descriptor at +0xA8 and descriptor+4 result
   at +0xAC; NO null check on the descriptor (crashes for unknown id) */

/* Equips a weapon on a player: looks up the weapon's descriptor by id, stores it in the player,
   then calls the descriptor's equip function (inferred from the name) with the player and
   stores its return value in the player. Parameters: player = player weapon struct*,
   weapon_id = weapon type id (WeaponId). Returns nothing. */

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


