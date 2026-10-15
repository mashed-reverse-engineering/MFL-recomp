/* Named constants for values that appear as magic numbers in the decompilation (all verified against the original code unless noted). */
#ifndef MFL_CONSTS_H
#define MFL_CONSTS_H

#define MAX_CARS 16               /* Car records player_car[16] (index checks `>= 0x10`) */
#define MAX_PLAYERS 4             /* player slots / vehicle records / AI bots */
#define WHEELS_PER_CAR 4
#define MAX_RWP_OBJECTS 200       /* RWP physics object table (rwp_object_* index checks) */
#define MAX_VIEWPORTS 4           /* viewport camera / raster table */

/* g_race_phase values (race_set_phase / race_get_phase) */
enum RacePhase {
    RACE_PHASE_SETUP = 1,         /* course_race_phase_hook: course marker positions are initialised */
    RACE_PHASE_PRE_LOAD = 2,
    RACE_PHASE_LOADING = 3,
    RACE_PHASE_COUNTDOWN = 5,     /* the deterministic test harness seeds the RNG here */
    RACE_PHASE_RACING = 6,
    RACE_PHASE_STANDINGS = 7
};

/* g_app_state (main loop state) */
enum AppState {
    APP_STATE_MAIN_LOOP_INIT = 1,
    APP_STATE_FRONTEND = 5        /* app_enter_frontend */
};

/* Lap_Line table markers (CourseLap.lap_lines, written by the Lua callbacks Lap_Line_End / Lap_Line_Change) */
#define LAP_LINE_END (-1)
#define LAP_LINE_CHANGE 0x100

/* course token list terminators / markers (course_token_list_*) */
#define TOKEN_LIST_END_RECORD (-1)
#define TOKEN_LIST_END_ALL (-10)
#define TOKEN_LIST_RANGE_MARKER 0x100

/* AI */
#define AI_BOT_TIMER_RESET 30000  /* ai_bot_set_timer_30000 */
#define AI_BOT_TIMER_NONE 1000    /* 0x3e8 written by ai_bot_state_reset */

/* menu / message ids */
#define MSG_CONTROLLER_MISSING 0x28a
#define MSGCODE_CONTROLLER_MISSING_A (-0xdf0000)
#define MSGCODE_CONTROLLER_MISSING_B (-0xde0000)
#define MENU_PAGE_OPTIONS 0x13    /* frontend page id of the options menu */

/* Weapon fire phases (fire callbacks: phase 2 = trigger pressed) */
#define WEAPON_PHASE_FIRE 2

#endif
