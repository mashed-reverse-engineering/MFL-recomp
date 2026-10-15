/* functions of game_misc.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of game_misc.c is kept as it was */
extern int FUN_00561060(void *);
extern char g_gamesave_5dd9b4[];
extern int g_fx_object_count, g_explosion_651aa4;
extern int g_fx_651260[]; /* stride 0xec bytes (0x3b dwords) */
extern int game_mode, g_car_652df8, g_intro_652df0, g_car_7ef7b0;
extern int g_race_phase;
extern float g_race_652de8;
extern int g_camera_7ef7d8;
extern char *g_race_setup;
extern int g_trail_653064[];
extern float g_noise_table_653298[];
extern char *g_current_course_entry;
extern char g_game_65a194[];
extern char g_game_65a168[];
extern int g_game_659f00[];
extern float g_attract_timer, g_attract_timer_64d8a8, g_attract_timer_64d8ac, g_scene_64d8b4;
extern float g_line_exceptions[0x480];
extern int g_btb_panel_658c0c[], g_btb_panel_658c10[];
extern int g_hudpanel_659aa4;
extern float g_hudpanel_659aa8;
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x00403860
void attract_timer_reset(void)
{
    g_attract_timer_64d8a8 = 0.0f;
    g_attract_timer = 0.0f;
    g_scene_64d8b4 = 40.0f;
    g_attract_timer_64d8ac = 60.0f;
}
