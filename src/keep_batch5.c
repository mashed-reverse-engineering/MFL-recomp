/* functions of batch5.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of batch5.c is kept as it was */
#pragma auto_inline(off)
struct KtcPickup { int timer; int timer_copy; int index; };
extern struct KtcPickup g_fx_pickups[]; /* 0x6516F0, 10 per group, 12 bytes each */
extern int *course_token_list_base(void);
extern void message_close(void);
extern int g_gamesave_load_step, g_gamesave_format_step, g_gamesave_result_code, g_gamesave_save_step, g_autosave_step;
extern int g_gamesave_delete_step, g_gamesave_operation, g_gamesave_status_poll_count, g_gamesave_flag_c0c;
extern int FID_conflict__wprintf(const char *, ...);
extern int g_fx_651948[];
extern int g_fx_6516f0[];
extern int powerup_place(int, int);
extern int player_has_weapon(int);
extern float g_tick_seconds, k_f_0;
extern int powerup_set_enabled(int, int);
extern char g_player_route_8d8d3c[];
extern int g_player_8d8ca8[];
extern int course_get_safe_start_count(void);
extern void course_get_lap_line(int, short *, short *);
extern int game_mode, g_crown_64d950;
extern float g_course_7ef7cc;
extern int vehicle_is_active(int), vehicle_part_ltm(int, int);
extern int RwFrameTransform(int, int, int);
extern void RpClumpRender(int);
extern void fx_object_timer_subtract(void);
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x00406b90
void ktc_add_pickup(int group, int a, int b, int timer)
{
    int n = g_fx_651948[group];
    struct KtcPickup *e = &g_fx_pickups[n + group * 10];
    int index = powerup_place(a, b);
    e->index = index;
    g_fx_651948[group] = n + 1;
    e->timer = timer;
    e->timer_copy = timer;
}

// FUNCTION: MFL 0x00404fc0
void crown_render(void)
{
    if (game_mode == 10 && g_crown_64d950 != 0 && g_course_7ef7cc > k_f_0) {
        if (vehicle_is_active(0) != 0) {
            int m = vehicle_part_ltm(0, 0x16);
            RwFrameTransform(*(int *)(g_crown_64d950 + 4), m, 0);
            RpClumpRender(g_crown_64d950);
        }
    }
}
