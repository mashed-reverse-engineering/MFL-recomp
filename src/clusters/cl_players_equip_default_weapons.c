// cluster-entries: players_equip_default_weapons
/* Installed cluster players_equip_default_weapons (tools/cluster_install.py from output/cluster_pilot6/players_equip_default_weapons.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster players_equip_default_weapons: for each of the 4 players that has status 1 but no weapon, equips the weapon given as the argument. */
extern int player_status_get(int player);
extern int player_has_no_weapon(int player);
extern void player_equip_weapon_by_index(int player, int weapon);

/* was: EDI = weapon index */
void players_equip_default_weapons_n(int weapon)
{
    int player;

    for (player = 0; player < 4; player++) {
        if (player_status_get(player) == 1 && player_has_no_weapon(player) != 0) {
            player_equip_weapon_by_index(player, weapon);
        }
    }
}

/* entry thunk at the original address of players_equip_default_weapons: register inputs (edi) -> natural C players_equip_default_weapons_n; ret 0 */
__declspec(naked) void players_equip_default_weapons(void)
{
    __asm {
        push edi
        call players_equip_default_weapons_n
        add esp, 4
        ret
    }
}
