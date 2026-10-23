/* MFL.exe player_np: functions moved verbatim from game_globals.c, game_misc2.c, round_stats.c, ui_misc.c, vehicles_stats.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x00408e80
int player_record_get_8d8ca8(int player)
{
    return g_player_8d8ca8[player * 0xc3];
}

// FUNCTION: MFL 0x0044c180
float player_distance_get(int player)
{
    if (player < 4)
        return distance_to_leader[player];
    return k_f_0;
}

// FUNCTION: MFL 0x0040cf10
void players_have_zero_points(int *out)
{
    int i;
    for (i = 0; i < 4; i++) {
        out[i] = 0;
        if (player_points[i] == 0)
            out[i] = 1;
    }
}

// FUNCTION: MFL 0x0040cd50
int player_get_stat(int player, int kind)
{
    switch (kind) {
    case 0:
        return g_player_8d8b90[player];
    case 1:
        return g_round_8d8bc0[player];
    case 2:
        return g_round_8d8ba0[player];
    case 3:
        return g_round_8d8bb0[player];
    default:
        return 0;
    }
}

// FUNCTION: MFL 0x00436520
int player_slots_count_active(void)
{
    int n = 0;
    if (g_player_slots[0].controller != -1)
        n = 1;
    if (g_player_slots[1].controller != -1)
        n++;
    if (g_player_slots[2].controller != -1)
        n++;
    if (g_player_slots[3].controller != -1)
        n++;
    return n;
}

// FUNCTION: MFL 0x0040c9c0
void player_add_wins(int player, int amount)
{
    if (player > 3)
        player = 3;
    g_player_8d8b50[player] += amount;
}

// FUNCTION: MFL 0x004276b0
int player_stat_get_crowns(int p)
{
    if (get_event_type() == EVENT_CUP_4)
        return 0;
    return g_player_stats_result[p].crowns;
}

// FUNCTION: MFL 0x004276d0
int player_stat_get_pants(int p)
{
    if (get_event_type() == EVENT_CUP_4)
        return 0;
    return g_player_stats_result[p].pants;
}

// FUNCTION: MFL 0x004276f0
int player_stat_get_crowns_total(int p)
{
    if (get_event_type() == EVENT_CUP_4)
        return 0;
    return g_player_stats_total[p].crowns;
}

// FUNCTION: MFL 0x00427710
int player_stat_get_pants_total(int p)
{
    if (get_event_type() == EVENT_CUP_4)
        return 0;
    return g_player_stats_total[p].pants;
}
