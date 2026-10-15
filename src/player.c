/* MFL.exe player: functions moved verbatim from batch13.c, batch21.c, batch22.c, batch26.c, batch5.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"
extern char g_race_start_8d98d8[];

// FUNCTION: MFL 0x00481e80
int player_marker_get_colour(int i)
{
    if (g_gfx_6c3f38[i * 4] != 0)
        return g_car_6c3f34[i * 4];
    return g_car_6c3f30[i * 4];
}

// FUNCTION: MFL 0x00481ea0
void player_marker_set_clump(int i, int clump)
{
    g_gfx_6c3f3c[i * 4] = clump;
}

// FUNCTION: MFL 0x00467890
void player_status_set(int i, int v)
{
    g_player_status[i] = v;
}

// FUNCTION: MFL 0x004678b0
int player_get_weapon_id(int i)
{
    struct WeaponSlot *s = &player_weapon_slots[i];
    if (s->descriptor != 0)
        return *(int *)s->descriptor;
    return -1;
}

// FUNCTION: MFL 0x00467eb0
int player_has_no_weapon(int i)
{
    return player_weapon_slots[i].descriptor == 0;
}

// FUNCTION: MFL 0x00468a60
int players_same_team_state(int a, int b)
{
    if (g_sound_player_count > 1 || b < 0)
        return 0;
    return g_player_slots[a].state == g_player_slots[b].state;
}

// FUNCTION: MFL 0x00426630
void player_stats_clear_a(void)
{
    int *p = (int *)g_player_stats_race;
    int n;
    for (n = 0x138; n != 0; n--)
        *p++ = 0;
}

// FUNCTION: MFL 0x00426650
void player_stats_clear_b(void)
{
    int *p = (int *)g_player_stats_total;
    int n;
    for (n = 0x138; n != 0; n--)
        *p++ = 0;
}

// FUNCTION: MFL 0x00427690
int player_get_last_hit_by(int p)
{
    return g_player_stats_race[p].last_hit_by;
}

// FUNCTION: MFL 0x00409f50
char *player_route_get_record_ptr(int p)
{
    return g_player_route_8d8d3c + p * 0x30c;
}

// FUNCTION: MFL 0x00408ec0
void player_records_clear_all(void)
{
    int *p = g_player_8d8ca8;
    int *q;
    int n;
    do {
        q = p - 10;
        for (n = 0xc3; n != 0; n--)
            *q++ = 0;
        *p = -1;
        p += 0xc3;
    } while ((int)p < ((int)&g_race_start_8d98d8));
}
