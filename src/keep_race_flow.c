/* functions of race_flow.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of race_flow.c is kept as it was */
#pragma auto_inline(off)
extern void race_set_phase(int);
extern void replay_records_reset(void);
extern char *g_race_setup;
extern int g_race_phase, g_camera_7ef7d8, game_mode, player_count, g_round_find_6037c4, g_phase_timer;
extern float g_race_652de8;
extern int get_event_type(void);
extern int race_get_setup_index(void);
extern int car_is_active(int);
extern int player_get_points(int);
extern int is_team_mode(void);
extern void time_set_current_max(void);
extern void intro_banner_set_active(int);
extern void replay_load_compare(int);
extern void game_set_game_659d00(int);
extern void race_course_loaded(char *setup, int index);
extern void race_ai_slot_assign_or_adjust(void);
extern void course_hook9(int);
extern void particle_pool_counts_reset(void);
extern void weapons_unequip_all_b(void);
extern void particle_templates_init_b(void);
extern void course_powerups_setup(void);
extern void scene_select_timer_clear(void);
extern void time_trial_record_step(int, int);
extern void race_time_trial_record(int);
extern int g_race_clock_a;
extern int game_mode_end_check(void);
extern int race_check_winner_by_game_mode(void);
extern void career_event_won(void);
extern void hudpanel_reset_all(void);
extern int physics_buffer_destroy(int);
extern void physics_buffer_rewind(int);
extern void rwp_world_clear_arena(int);
extern void time_split(int, int *, int *, int);
extern int g_replay_652f7c, g_replay_652f80, g_replay_652f8c, g_replay_652f88, g_replay_652fa4, g_time_652f94;
extern void cup_scene_update_players(void);
extern void cup_scene_render_players(void);
extern float k_f_3_3333333en4;
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x0040ff70
int race_get_last_active_player(void)
{
    int last = -1;
    int count = 0;
    int slot = 0;
    int off = 0xb8;
    do {
        if (*(int *)(off + (int)g_race_setup) != 0) {
            if (car_is_active(slot) == 1) {
                count++;
                last = slot;
            }
        }
        off += 4;
        slot++;
    } while (off < 200);
    if (count == 1)
        return last;
    return -1;
}
