/* functions of misc3.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of misc3.c is kept as it was */
extern int get_event_type(void);
extern int current_course_id(void);
extern int car_is_active(int car);
extern int g_scene_selected_object, g_scene_select_param;
extern float g_scene_select_timer;
extern int g_fx_651260[]; /* fx object records, 0x3b dwords */
extern int g_scene_651294[], g_fx_object_count;
extern int g_fx_651298[];
extern int g_round_652dbc;
extern int g_gamesave_load_step, g_gamesave_operation;
extern struct { int mirror_mode; } g_game_state_mirror;
extern int g_replay_652f88;
extern void replay_get_split_time(int replay, int idx, int a, int b, int c);
extern char g_career_event_608d38[], g_career_event_608e28[], g_career_event_608f18[], g_career_event_608a68[];
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x0040d000
void cars_inactive_flags(int *out)
{
    int i = 0;
    do {
        out[i] = 0;
        if (car_is_active(i) == 0)
            out[i] = 1;
        i++;
    } while (i < 4);
}
