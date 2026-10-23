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
#ifndef MFL_APP_STATE_ENUM
#define MFL_APP_STATE_ENUM
/* g_app_state (0x0076e7f4, get_app_state()): the main loop state machine of the frontend frame step (update and draw per state).
   Values derived from what each case does; plain int constants. */
enum AppState {
    APP_STATE_NONE = 0,           /* before main_loop_init (the music code treats it as "no game": fades out) */
    APP_STATE_MAIN_LOOP_INIT = 1, /* set by main_loop_init: title / attract menu (menu ids 0, 1, 3, 0x21) */
    APP_STATE_COURSE_LOADED = 2,  /* course_load_time_report, then straight on to the race; renders nothing */
    APP_STATE_RACE = 3,           /* race running (training_mode_update); sound voices are only positioned in this state */
    APP_STATE_MENU = 4,           /* frontend page drawn alone (plus the options menu for menu id 0x20) */
    APP_STATE_FRONTEND = 5,       /* app_enter_frontend: race drawn with the frontend on top (pause / results), waits for ui state 2 */
    APP_STATE_RACE_START = 6,     /* chunked race_start_setup loop, frontend over the race; ends when ui state 0 */
    APP_STATE_RACE_MESSAGE = 7    /* race with a message box up (message_show_*) */
};
#endif

#ifndef MFL_EVENT_ENUMS
#define MFL_EVENT_ENUMS
/* g_event_type (0x0069cbd4, get_event_type()): the career grid column of the selected event (0..0x12).  Names follow the icon
   medal_asset_names picks per event (TimeTrial / cup / MultiPlayer / QuickRace / tournament icons); 9 and 10 are not identified. */
enum EventType {
    EVENT_TIME_TRIAL_0 = 0,        /* "TimeTrial" icon */
    EVENT_TIME_TRIAL_1 = 1,
    EVENT_TIME_TRIAL_2 = 2,
    EVENT_CUP_3 = 3,               /* cup events: "BronzeCup" / "SilverCup" / "GoldCup" and the medal icons, special stages (GameMode 5, 7..10) */
    EVENT_CUP_4 = 4,
    EVENT_CUP_5 = 5,
    EVENT_MULTIPLAYER_6 = 6,       /* "MultiPlayer" icon; battle events with 2 opponents left end the round */
    EVENT_MULTIPLAYER_7 = 7,
    EVENT_MULTIPLAYER_8 = 8,
    EVENT_QUICK_RACE_11 = 0xb,     /* "QuickRace" icon; bot skill from menu_bot_skill_b */
    EVENT_QUICK_RACE_12 = 0xc,
    EVENT_QUICK_RACE_13 = 0xd,
    EVENT_TURBO_TOURNAMENT = 0xe,  /* medal_asset_names: "TurboT" icon */
    EVENT_ICE_TOURNAMENT = 0xf,    /* "IceT" */
    EVENT_NIGHT_TOURNAMENT = 0x10, /* "NightT" */
    EVENT_TRAINING = 0x11,         /* the training lessons (training_lesson_get branches, training prompt, no course overlay) */
    EVENT_QUICK_RACE = 0x12        /* set by quick_race_defaults_set together with GAME_MODE_QUICK_RACE */
};
/* game_mode (0x007ef7b8): the rule set of the race.  Names from the text the game shows for the mode (Font36/ENGLISH.DAT: the
   "special stage" titles of menu_draw_event_select, selected by the career kind code that menu_update maps to the mode) and from the code
   that runs only in the mode. */
enum GameMode {
    GAME_MODE_STANDARD = 0,        /* "Standard Play" (multiplayer game mode choice 0, battle events) */
    GAME_MODE_HOLD_THE_FLAG = 1,   /* choice 1: only the flag holder (g_cars_7ef7bc, the "leader" / fugitive) scores, car_hit_by_leader passes the flag (inferred) */
    GAME_MODE_HARE_AND_HOUNDS = 2, /* choice 2: one special car (g_cars_7ef7c0, grid slot 100) is chased (inferred) */
    GAME_MODE_RACE = 4,            /* "Race Game: 3 lap race against N opponents" (career kinds 2, 3, 0xb) */
    GAME_MODE_KILL_COPTER = 5,     /* special stage "Kill-the-Copter" (copter models are loaded in this mode) */
    GAME_MODE_QUICK_RACE = 6,      /* quick_race_defaults_set */
    GAME_MODE_AIRSTRIKE = 7,       /* special stage "Air Strike", no HUD */
    GAME_MODE_SURVIVAL = 8,        /* special stage "Survival Game" */
    GAME_MODE_CHASE_FUGITIVE = 9,  /* special stage "Chase-the-Fugitive": one human, scripted car, time limit text */
    GAME_MODE_BEAT_THE_BOMB = 10   /* special stage "Beat-the-Bomb": bomb timer text, bomb model (Bomb.dff) */
};
/* menu page ids (g_menu_records[].page_id, menu_page_enter): named after the title / item texts of the page tables at g_menu_page_item_lists
   (text id = Font36/DEFINES.TXT value + 1).  Pages 0x14, 0x16, 0x17, 0x19, 0x1b, 0x26, 0x27 are not identified. */
enum MenuPage {
    MENU_PAGE_PAUSE = 0,           /* retry / options / restart race / quit race */
    MENU_PAGE_MAIN = 1,            /* single player / multiplayer / options / bonus features / exit */
    MENU_PAGE_SINGLE_PLAYER = 2,   /* training / challenge cup / arcade modes / tournaments */
    MENU_PAGE_MULTIPLAYER = 3,     /* standard play / team play */
    MENU_PAGE_TEAM_COLOUR_SELECT = 4,
    MENU_PAGE_RACE_RESULTS = 5,
    MENU_PAGE_CHALLENGE_SELECT_A = 6,
    MENU_PAGE_CHALLENGE_SELECT_B = 7,
    MENU_PAGE_OPTIONS = 8,         /* sound / gamma correction / load-save */
    MENU_PAGE_TEAMS = 9,           /* free play / custom */
    MENU_PAGE_CUSTOMISE_PLAY = 0xa,
    MENU_PAGE_XBOX_LIVE = 0xb,     /* host / join */
    MENU_PAGE_LOBBY_A = 0xc,
    MENU_PAGE_JOIN = 0xd,
    MENU_PAGE_LOBBY_B = 0xe,
    MENU_PAGE_ABILITY_SELECT = 0xf,
    MENU_PAGE_TEAM_SELECT = 0x10,
    MENU_PAGE_CUSTOM_ABILITIES = 0x11, /* ability / airstrike / game length */
    MENU_PAGE_GAME_SETUP_MULTI = 0x12, /* play game / power-ups / game length / extra opponents / vehicle / game mode */
    MENU_PAGE_SOUND = 0x13,        /* music / sfx / insults volume sliders (g_game_state.option_sliders) */
    MENU_PAGE_GAME_SETUP_SINGLE = 0x18, /* play game / power-ups / difficulty / vehicle / opponents / game mode */
    MENU_PAGE_GAME_OPTIONS = 0x1a, /* move screen */
    MENU_PAGE_VIBRATION = 0x1c,
    MENU_PAGE_CONTROLLERS = 0x1d,
    MENU_PAGE_GAMMA = 0x1e,        /* PC gamma correction */
    MENU_PAGE_BONUS_FEATURES = 0x1f, /* flipped / oz / retro / mashed camera */
    MENU_PAGE_AUTOSAVE = 0x20,
    MENU_PAGE_BONUS_CAMERAS = 0x21,
    MENU_PAGE_ARCADE_MODES = 0x22, /* quick race / time attack */
    MENU_PAGE_WIDESCREEN = 0x23,
    MENU_PAGE_SCREEN = 0x24,
    MENU_PAGE_LOAD_SAVE = 0x25
};
#endif

#ifndef MFL_WEAPON_ENUMS
#define MFL_WEAPON_ENUMS
/* weapon type ids (player_equip_weapon / player_get_weapon_id / powerup_random_weapon_type); only the ids that code names. */
enum WeaponType {
    WEAPON_NONE = -1,
    WEAPON_MORTAR = 7,
    WEAPON_MACHINEGUN = 9,
    WEAPON_DEPTHCHARGE = 10,
    WEAPON_MISSILE = 11,
    WEAPON_MINE = 12,
    WEAPON_FLAMETHROWER = 16,
    WEAPON_SHOTGUN = 17,
    WEAPON_FLARE = 18,
    WEAPON_OIL = 19,
    WEAPON_CHAOS = 21,
    WEAPON_AIRSTRIKE = 22
};
/* bits of the vehicle record flag word set by vehicle_set_flag_* (FRONT / BACK inferred, the others only known as bit values) */
enum VehicleFlag {
    VEHICLE_FLAG_ACTIVE = 0x8,     /* vehicle_set_active */
    VEHICLE_FLAG_FRONT = 0x10,
    VEHICLE_FLAG_BACK = 0x20,
    VEHICLE_FLAG_40 = 0x40,
    VEHICLE_FLAG_80 = 0x80,
    VEHICLE_FLAG_400 = 0x400,
    VEHICLE_FLAG_800 = 0x800,
    VEHICLE_FLAG_1000 = 0x1000,
    VEHICLE_FLAG_2000 = 0x2000
};
#endif

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

/* Weapon fire phases (fire callbacks: phase 2 = trigger pressed) */
#define WEAPON_PHASE_FIRE 2

#endif
