// cluster-entries: kc_panel_entry_init kc_panel_init
/* Installed cluster kc_panel_entry_init (tools/cluster_install.py from output/cluster_pilot/kc_panel_entry_init.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/* DECLS-END */
/* Cluster kc_panel_entry_init: per player entries of the KC HUD panel (4 records of 0x114 bytes). */
typedef struct KcPanelEntry {
    char unknown_00[0x108];
    int player_index;      /* index into g_player_slots */
    uint flags;            /* 1 << team, | 0x400 or | 0x800 (bit set in team / multiplayer modes) */
    int is_extra;          /* 1 when the player index is below the player count */
} KcPanelEntry;

extern KcPanelEntry g_kc_659340[4];
extern int g_kc_panel_659304;      /* player count */
extern int max_points_b;           /* score target */
extern int game_mode;
extern int get_player_count(void);
extern int score_target_a(void);
extern int is_team_mode(void);

/* Fills flags of one panel entry (was: ESI = entry). */
void kc_panel_entry_init_n(KcPanelEntry *entry)
{
    uint flags;

    entry->flags = 0;
    entry->is_extra = 0;
    flags = 1u << (g_player_slots[entry->player_index].team & 31);
    entry->flags = flags;
    if (g_kc_panel_659304 > 3 && is_team_mode() == 0 && game_mode != 1 && game_mode != 2) {
        flags |= 0x800;
    } else {
        flags |= 0x400;
    }
    entry->flags = flags;
    if (entry->player_index < g_kc_panel_659304) {
        entry->is_extra = 1;
    }
}

/* Reads the player count and score target and initialises the four entries. */
void kc_panel_init(void)
{
    int i;

    g_kc_panel_659304 = get_player_count();
    max_points_b = score_target_a();
    for (i = 0; i < 4; i++) {
        kc_panel_entry_init_n(&g_kc_659340[i]);
    }
}

/* entry thunk at the original address of kc_panel_entry_init: register inputs (esi) -> natural C kc_panel_entry_init_n; ret 0 */
__declspec(naked) void kc_panel_entry_init(void)
{
    __asm {
        push esi
        call kc_panel_entry_init_n
        add esp, 4
        ret
    }
}
