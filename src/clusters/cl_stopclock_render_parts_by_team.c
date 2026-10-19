// cluster-entries: stopclock_render_parts_by_team stopclock_render
/* Installed cluster stopclock_render_parts_by_team (tools/cluster_install.py from output/cluster_pilot5/stopclock_render_parts_by_team.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/* DECLS-END */
/* Cluster stopclock_render_parts_by_team: draws the parts of the stop clock panel (13 object pointers, +0x00..), the team part of player 0 and optionally the part chosen by +0x50. */
extern char g_hudpanel_659ac0[];
extern int g_stopclock_659b08;

static void part_draw(void *part)
{
    void (*fn)(void *) = *(void (**)(void *))((char *)part + 0x48);
    fn(part);
}

/* was: ESI = panel, flag on the stack */
void stopclock_render_parts_by_team_n(void **panel, int extra)
{
    part_draw(panel[13]);
    part_draw(panel[2]);
    part_draw(panel[0]);
    part_draw(panel[1]);
    part_draw(panel[7 + g_player_slots[0].team]);
    if (extra != 0) {
        part_draw(panel[2 + (int)panel[0x14]]);
    }
}

void stopclock_render(int extra)
{
    if (g_stopclock_659b08 != 0) {
        stopclock_render_parts_by_team_n((void **)g_hudpanel_659ac0, extra);
    }
}

/* entry thunk at the original address of stopclock_render_parts_by_team: register inputs (esi,s0) -> natural C stopclock_render_parts_by_team_n; ret 0 */
__declspec(naked) void stopclock_render_parts_by_team(void)
{
    __asm {
        push dword ptr [esp + 4]
        push esi
        call stopclock_render_parts_by_team_n
        add esp, 8
        ret
    }
}
