# Named constants (enums)

The magic numbers of the installed C are replaced by enum constants where the meaning follows from the code that uses the value
(and, for the app state, from what each case of the state machine does).  Pure rename: the compiled bytes are unchanged.
Defined once per consumer family, identical text, each block guarded against double inclusion:

| header | consumers |
|---|---|
| `recomp/mfl_tier2.h` (copies in `recomp/automatch_near/`, `recomp/clusters/`, symlink in `recomp/tier1/`) | tier 2, clusters, and the tier 1 files that include it |
| `recomp/auto/prelude.h` (copies in the other source directories) | tier 1 / agent sources that include `prelude.h` |
| `recomp/mfl_consts.h` | the hand written modules (through `mfl_externs.h`) |

Values that are not evident stay literals or get a neutral `<NAME>_<n>` constant.

## AppState (`g_app_state`, `get_app_state()`)

| value | name | meaning |
|---|---|---|
| 0 | `APP_STATE_NONE` | before `main_loop_init`; the music code treats it as "no game" |
| 1 | `APP_STATE_MAIN_LOOP_INIT` | set by `main_loop_init`: title / attract menu (menu ids 0, 1, 3, 0x21) |
| 2 | `APP_STATE_COURSE_LOADED` | `course_load_time_report`, then on to the race; draws nothing |
| 3 | `APP_STATE_RACE` | race running; sound voices are only positioned in this state |
| 4 | `APP_STATE_MENU` | frontend page drawn alone (options menu for menu id 0x20) |
| 5 | `APP_STATE_FRONTEND` | `app_enter_frontend`: race with the frontend on top (pause, results) |
| 6 | `APP_STATE_RACE_START` | chunked `race_start_setup` loop, frontend over the race |
| 7 | `APP_STATE_RACE_MESSAGE` | race with a message box |

The `g_app_state_<addr>` symbols are NOT states: they are unrelated globals (frame timing, hint latches, label buffers) that the symbol
namer put in the same bucket; they are left alone.

## EventType (`g_event_type`, `get_event_type()`)

Names follow the icon `medal_asset_names` picks for the event (exe strings "TimeTrial", "BronzeCup".., "MultiPlayer", "QuickRace", "TurboT"/"IceT"/"NightT").

| value | name |
|---|---|
| 0, 1, 2 | `EVENT_TIME_TRIAL_0..2` |
| 3, 4, 5 | `EVENT_CUP_3..5` (cup events; the special stages run inside them) |
| 6, 7, 8 | `EVENT_MULTIPLAYER_6..8` |
| 9, 10 | not identified (literals) |
| 0xb, 0xc, 0xd | `EVENT_QUICK_RACE_11..13` |
| 0xe / 0xf / 0x10 | `EVENT_TURBO_TOURNAMENT` / `EVENT_ICE_TOURNAMENT` / `EVENT_NIGHT_TOURNAMENT` |
| 0x11 | `EVENT_TRAINING` |
| 0x12 | `EVENT_QUICK_RACE` (`quick_race_defaults_set`) |

## GameMode (`game_mode`)

Evidence: `menu_update` maps a career "kind" code (`entry & 0xffff0000`) to the mode, and `menu_draw_event_select` shows the title text for
that code; the texts are in `Common/Font36/ENGLISH.DAT` (string index = `DEFINES.TXT` value + 1).

| value | name | evidence |
|---|---|---|
| 0 | `GAME_MODE_STANDARD` | "Standard Play", game mode choice 0 |
| 1 | `GAME_MODE_HOLD_THE_FLAG` | choice 1; only the flag holder scores, `car_hit_by_leader` passes the flag (inferred) |
| 2 | `GAME_MODE_HARE_AND_HOUNDS` | choice 2; one special car (grid slot 100) is chased (inferred) |
| 4 | `GAME_MODE_RACE` | kinds 2, 3, 0xb: "Race Game: 3 lap race against N opponents" |
| 5 | `GAME_MODE_KILL_COPTER` | kind 7, "Kill-the-Copter Special"; copter models loaded only in this mode |
| 6 | `GAME_MODE_QUICK_RACE` | `quick_race_defaults_set` |
| 7 | `GAME_MODE_AIRSTRIKE` | kind 8, "Air Strike Special" |
| 8 | `GAME_MODE_SURVIVAL` | kind 6, "Survival Game Special" |
| 9 | `GAME_MODE_CHASE_FUGITIVE` | kind 5, "Chase-the-Fugitive Special" |
| 10 | `GAME_MODE_BEAT_THE_BOMB` | kind 4, "Beat-the-Bomb Special"; Bomb.dff, bomb timer |

## MenuPage (`g_menu_records[].page_id`, `menu_page_enter`)

Named after the title / item texts of the page tables at `g_menu_page_item_lists` (0x60aca8): `PAUSE` 0, `MAIN` 1, `SINGLE_PLAYER` 2,
`MULTIPLAYER` 3, `TEAM_COLOUR_SELECT` 4, `RACE_RESULTS` 5, `CHALLENGE_SELECT_A/B` 6/7, `OPTIONS` 8, `TEAMS` 9, `CUSTOMISE_PLAY` 0xa,
`XBOX_LIVE` 0xb, `LOBBY_A` 0xc, `JOIN` 0xd, `LOBBY_B` 0xe, `ABILITY_SELECT` 0xf, `TEAM_SELECT` 0x10, `CUSTOM_ABILITIES` 0x11,
`GAME_SETUP_MULTI` 0x12, `SOUND` 0x13 (the former "options page" macros: it holds the volume sliders), `GAME_SETUP_SINGLE` 0x18,
`GAME_OPTIONS` 0x1a, `VIBRATION` 0x1c, `CONTROLLERS` 0x1d, `GAMMA` 0x1e, `BONUS_FEATURES` 0x1f, `AUTOSAVE` 0x20, `BONUS_CAMERAS` 0x21,
`ARCADE_MODES` 0x22, `WIDESCREEN` 0x23, `SCREEN` 0x24, `LOAD_SAVE` 0x25.  Pages 0x14, 0x16, 0x17, 0x19, 0x1b, 0x26, 0x27 are not identified.
The values of `g_menu_screen_id` (`ui_get_menu_69d1ec`: 0, 1, 2, 3, 9, 0x1c, 0x20, 0x21, 0x22) are a different space (UI / transition states) and stay literals.

## WeaponType

`WEAPON_NONE` -1, `MORTAR` 7, `MACHINEGUN` 9, `DEPTHCHARGE` 10, `MISSILE` 11, `MINE` 12, `FLAMETHROWER` 16, `SHOTGUN` 17, `FLARE` 18,
`OIL` 19, `CHAOS` 21, `AIRSTRIKE` 22 (ids 8, 20 and below 7 are not identified).  Car / vehicle ids and pickup ids do not occur as literals.

## VehicleFlag (vehicle record flag word)

`VEHICLE_FLAG_ACTIVE` 0x8 (`vehicle_set_active`), `FRONT` 0x10, `BACK` 0x20 (inferred), `_40`, `_80`, `_400`, `_800`, `_1000`, `_2000` (bit values only).
