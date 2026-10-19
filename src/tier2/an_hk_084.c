#include "mfl_tier2.h"
/*00467e60*/ extern int __cdecl player_equip_weapon(char *slot, int weapon_id);
/*008bed20*/ extern char player_weapon_slots[];

#define PLAYER_WEAPON_SLOT_STRIDE (0xb4)

/* player_equip_weapon_by_index */

/* cdecl (player index; weapon type id) -> player_equip_weapon */

void __cdecl player_equip_weapon_by_index(int player_index, int weapon_id)

{
  /* the original turns the index into the address of the player's 0xb4 byte weapon slot and tail-jumps
     to player_equip_weapon (two stack arguments: the first is rewritten in place, the second, the weapon id, passes through) */
  player_equip_weapon(player_weapon_slots + player_index * PLAYER_WEAPON_SLOT_STRIDE, weapon_id);
  return;
}
