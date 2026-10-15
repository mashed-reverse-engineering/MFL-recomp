/* MFL.exe race_np: functions moved verbatim from game_globals.c, game_misc.c, misc3.c, round_stats.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x0042e960
void intro_banner_set_active(int value)
{
    if (value == 0 || g_course_8c879c == 0)
        g_course_8c879c = value;
}

// FUNCTION: MFL 0x0040f710
int intro_banner_get_flag(void)
{
    if (game_mode != 10 && game_mode != 5)
        return g_car_652df8;
    return 0;
}

// FUNCTION: MFL 0x0040f7e0
unsigned int intro_banner_get_value(void)
{
    return (g_intro_652df0 != 1) - 1 & g_car_7ef7b0;
}

// FUNCTION: MFL 0x0040fe20
int race_setup_slot_used(int slot)
{
    if (slot >= 4)
        return 0;
    return *(int *)(g_race_setup + 0xb8 + slot * 4) != 0;
}

// FUNCTION: MFL 0x0040ff40
void race_setup_set_slot_type(int slot, int type)
{
    *(int *)(g_race_setup + 0xb8 + slot * 4) = type;
}

// FUNCTION: MFL 0x0040cc70
void round_time_accumulate(int ticks)
{
    if (get_event_type() != 0) {
        if (get_event_type() != 1) {
            if (get_event_type() != 2)
                g_round_652dbc = g_round_652dbc + ticks;
        }
    }
}

// FUNCTION: MFL 0x00413ad0
int replay_best_get_split_time(int idx, int a, int b, int c)
{
    if (g_replay_652f88 != 0 && *(int *)(g_replay_652f88 + 0x17c + idx * 4) != 0) {
        replay_get_split_time(g_replay_652f88, idx, a, b, c);
        return 1;
    }
    return 0;
}

// FUNCTION: MFL 0x0040cfa0
void round_points_clamp_to_limits(int *out)
{
    int limit = 99;
    int i;
    if (player_points[0] < 99)
        limit = player_points[0];
    if (g_round_8d8b44 < limit)
        limit = g_round_8d8b44;
    if (g_round_8d8b48 < limit)
        limit = g_round_8d8b48;
    if (g_round_8d8b4c < limit)
        limit = g_round_8d8b4c;
    for (i = 0; i < 4; i++) {
        out[i] = 0;
        if (player_points[i] == limit)
            out[i] = 1;
    }
}

void round_scores_reset(void)
{
    g_round_8d8b60 = g_round_8d8b64 = g_round_8d8b68 = -1000;
    g_round_8d8b80 = g_round_8d8b84 = g_round_8d8b88 = -1000;
    g_round_8d8b6c = g_round_8d8b8c = -1000;
}
