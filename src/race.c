/* MFL.exe race: functions moved verbatim from batch25.c, batch7.c, batch9.c, race_flow.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x00413a10
int replay_buffer_free(void)
{
    (*(void (**)(int))(RwEngineInstance + 0x10c))(g_replay_buffer);
    return 1;
}

// FUNCTION: MFL 0x004146f0
int race_event_has_panel(void)
{
    switch (get_event_type()) {
    case 0:
    case 1:
    case 2:
        return 3;
    case 3:
    case 4:
    case 5:
        switch (game_mode) {
        case 4:
        case 8:
        case 9:
            return 1;
        case 10:
            return 6;
        }
    }
    return 0;
}

// FUNCTION: MFL 0x0042e9e0
void time_clear_current(void)
{
    g_course_69bb6c = 0.0f;
    g_course_69bb64 = 0;
    g_course_69bb5c = 0;
    g_course_8c879c = 0;
}

// FUNCTION: MFL 0x0040e680
int race_setup_get_course(int slot)
{
    return *(int *)(*(int *)(g_race_setup + 4 + slot * 4) + 4);
}

// FUNCTION: MFL 0x0040ff20
int race_setup_get_slot_type(int slot)
{
    return *(int *)(g_race_setup + 0xb8 + slot * 4);
}

// FUNCTION: MFL 0x0040ffc0
int round_find_winner(void)
{
    int i;
    int points;
    g_round_find_6037c4 = -1;
    i = 0;
    while ((points = player_get_points(i), player_count != 2 && player_count != 3 && is_team_mode() == 0) || points != 8) {
        if (player_count == 4 && 0xb < points) {
            g_round_find_6037c4 = i;
            return i;
        }
        i++;
        if (3 < i)
            return g_round_find_6037c4;
    }
    g_round_find_6037c4 = i;
    return i;
}

// FUNCTION: MFL 0x00410020
void race_begin_countdown(void)
{
    race_set_phase(4);
    g_phase_timer = 12000;
}

/* naked: the original touches only EAX and the FPU, callers keep values in ECX/EDX across it (see tools/clobber.py) */
// FUNCTION: MFL 0x0040fdd0
__declspec(naked) void race_set_phase(int phase)
{
    __asm {
        mov eax, [esp+4]
        cmp g_race_phase, eax
        mov g_race_phase, eax
        je done
        fild g_camera_7ef7d8
        fmul k_f_3_3333333en4
        fstp g_race_652de8
    done:
        ret
    }
}

// FUNCTION: MFL 0x00410060
void race_finish(void)
{
    time_set_current_max();
    intro_banner_set_active(0xb);
    replay_load_compare(0);
    game_set_game_659d00(0);
    race_set_phase(7);
}

// FUNCTION: MFL 0x00410090
int race_get_course_id(void)
{
    int i = race_get_setup_index();
    return **(int **)(g_race_setup + 4 + i * 4);
}

// FUNCTION: MFL 0x00412d10
void race_phase_9_update(void)
{
    race_ai_slot_assign_or_adjust();
    {
        int i = race_get_setup_index();
        course_hook9(**(int **)(g_race_setup + 4 + i * 4));
    }
    particle_pool_counts_reset();
    weapons_unequip_all_b();
    particle_templates_init_b();
    if (game_mode == 10) {
        course_powerups_setup();
        return;
    }
}

// FUNCTION: MFL 0x00412d60
void race_restart(void)
{
    race_phase_9_update();
    replay_records_reset();
    race_set_phase(1);
    scene_select_timer_clear();
}

// FUNCTION: MFL 0x00413680
void race_phase_6_update(int arg)
{
    race_time_trial_record(g_race_clock_a);
    time_trial_record_step(arg, g_race_clock_a);
    if (game_mode_end_check() == 0)
        return;
    if (race_check_winner_by_game_mode() != 0) {
        career_event_won();
        return;
    }
    race_set_phase(7);
    hudpanel_reset_all();
}

// FUNCTION: MFL 0x00413a30
void replay_free_buffers(void)
{
    if (g_replay_652f7c != 0)
        g_replay_652f7c = physics_buffer_destroy(g_replay_652f7c);
    if (g_replay_652f80 != 0)
        g_replay_652f80 = physics_buffer_destroy(g_replay_652f80);
    g_replay_652f8c = 0;
    g_replay_652f88 = 0;
    g_replay_652fa4 = 0;
}

// FUNCTION: MFL 0x00413a80
void replay_get_split_time(int replay, int index, int *a, int *b, int c)
{
    if (replay != 0) {
        time_split(*(int *)(replay + 0x17c + index * 4), a, b, c);
        return;
    }
    *b = 0;
    *a = 0;
}

// FUNCTION: MFL 0x00413b10
int replay_current_get_split_time(int index, int *a, int *b, int c)
{
    if (g_replay_652f8c != 0 && *(int *)(g_replay_652f8c + 0x17c + index * 4) != 0) {
        replay_get_split_time(g_replay_652f8c, index, a, b, c);
        return 1;
    }
    return 0;
}

// FUNCTION: MFL 0x00413cb0
void replay_records_reset(void)
{
    int e = get_event_type();
    if (-1 < e && e < 3) {
        if (g_replay_652f88 != 0)
            physics_buffer_rewind(g_replay_652f88);
        if (g_replay_652f8c != 0) {
            rwp_world_clear_arena(g_replay_652f8c);
            *(int *)(g_replay_652f8c + 0x17c) = 0;
            *(int *)(g_replay_652f8c + 0x180) = 0;
        }
        g_time_652f94 = 0;
    }
}

// FUNCTION: MFL 0x0040f6a0
void race_cup_scene_update_players_if_event(void)
{
    switch (get_event_type()) {
    case 3:
    case 4:
    case 5:
    case 0xe:
    case 0xf:
    case 0x10:
        if (game_mode == 4 || (6 < game_mode && game_mode < 0xb)) {
            cup_scene_update_players();
            return;
        }
    }
}

// FUNCTION: MFL 0x0040f940
void race_phase_5_6_render_by_event(void)
{
    if (4 < g_race_phase && g_race_phase < 7) {
        switch (get_event_type()) {
        case 3:
        case 4:
        case 5:
        case 0xe:
        case 0xf:
        case 0x10:
            switch (game_mode) {
            case 4:
            case 7:
            case 8:
            case 9:
            case 10:
                cup_scene_render_players();
                return;
            }
        }
    }
}
