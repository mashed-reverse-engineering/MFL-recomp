# How the game is put together

A newcomer's map of `MFL.exe` as far as this project has named it. Function names come from `names/MFL_*.csv` (address, name, comment);
the C is in `src/` (hand-written modules and the manifest), `src/tier2/` and `src/clusters/` (verified rewrites).
Every name below is a real function or constant of the published sources or name tables. Not every function has been rewritten
yet: where the C is not published, the entry says "name table only" and you find it by name in `names/`.

Confidence: statements without a mark follow directly from the code; **(inferred)** means the reading is a likely interpretation
of how the code is used, not proven. Constants are listed in [ENUMS.md](ENUMS.md), record layouts in [STRUCTS.md](STRUCTS.md).

## 1. Process and main loop

1. Startup creates the window and the RenderWare engine, then `app_boot_init` (`src/clusters/cl_app_boot_init.c`) loads the
   resources: D3D9 pipeline, the PIZ archive file system, render pipelines, the alpha test state, the intro video, fonts and the
   language table, then the HUD panels (`hudpanel_load_assets_0041b960`, `score_panel_load`, `load_team_panels`, `load_kc_panel`,
   `load_start_lights`, ...), car shadow and damage setup, and the cup scene models. It returns 0 if the pipeline cannot be created.
2. `app_main_loop` (name table only) runs after the assets are loaded. It is a loop of frames: `app_frame_timing` measures the
   delta of `timer_ticks`, `app_frame_wait` waits until the frame time is reached, and the frame is dispatched on `g_app_state`.
3. `main_loop_init` (name table only) resets the quit flag and frame counters and sets the state to `APP_STATE_MAIN_LOOP_INIT`.
4. `app_state_transitions` (name table only) is a switch over the states 1..7 that decides the next state. Per state a frame is
   `app_state_frontend_frame` (`src/tier2/an_hk_789.c`: one `game_tick` plus `menu_update`) or `app_state_menu_render`
   (`src/tier2/an_hk_611.c`: camera clear and menu render).

The application state (`enum AppState` in [ENUMS.md](ENUMS.md)): `APP_STATE_NONE` 0, `MAIN_LOOP_INIT` 1, `COURSE_LOADED` 2,
`RACE` 3, `MENU` 4, `FRONTEND` 5, `RACE_START` 6, `RACE_MESSAGE` 7. `app_enter_frontend` (`src/system.c`) restores the menu state,
enters the frontend and sets `APP_STATE_FRONTEND` (the race keeps running underneath for pause and results).

Simulation runs on a fixed step: `game_tick` (name table only) is called with 50-tick slices (1/60 s per iteration, **inferred** from
the constants) and dispatches on the race phase. `race_update_common` (`src/clusters/cl_race_update_common.c`) is the per tick work
shared by all phases: simulation time, `weapons_update_frame`, effects, and the total time clamp.

## 2. Race phases (`enum RacePhase`)

`g_race_phase` is changed only through `race_set_phase` (`src/race.c`), which also stamps the phase start time.

| phase | name | what happens | handler |
|---|---|---|---|
| 1 | `RACE_PHASE_SETUP` | course marker positions are initialised | `course_race_phase_hook` (`src/course.c`) |
| 2 | `RACE_PHASE_PRE_LOAD` | nothing of its own | `course_race_phase_hook` |
| 3 | `RACE_PHASE_LOADING` | the camera path plays; then phase 4 (or 6 in training) | `race_phase_3_update` (`src/clusters/cl_race_phase_3_update.c`) |
| 4 | `RACE_PHASE_INTRO` (inferred) | 12000 ms timer set by `race_begin_countdown` (`src/race.c`), waits for the countdown | `race_phase_4_update` (`src/tier2/an_hk_557.c`) |
| 5 | `RACE_PHASE_COUNTDOWN` | start lights; turbo charge is built up | `race_phase_5_update` (`src/clusters/cl_race_phase_5_update.c`) |
| 6 | `RACE_PHASE_RACING` | input is live; ends via `game_mode_end_check` | `race_phase_6_update` (`src/race.c`) |
| 7 | `RACE_PHASE_STANDINGS` | race clocks stopped, standings shown | `race_phase_7_update` (`src/tier2/an_hk_844.c`) |
| 8 | `RACE_PHASE_FINISHED` (inferred) | race ended without a winner camera | `race_check_winner_by_game_mode` |
| 9 | `RACE_PHASE_WINNER_VIEW` (inferred) | camera on the winner | `race_phase_9_update` (`src/race.c`) |
| 10 | `RACE_PHASE_WINNER_DONE` (inferred) | set right after phase 9; cars stop updating | event handler |
| 11 | `RACE_PHASE_ROUND_END` (inferred) | weapons/powerups off, end-of-round camera | `race_check_winner_by_game_mode` (`src/clusters/cl_race_check_winner_by_game_mode.c`) |

Flow of one race (from the code of the handlers): SETUP, load the course (section 5), spawn the cars (`race_spawn_cars`, name table only:
`car_set_bounds`, `car_init_dynamics`, `car_reset_state`, `grid_position`, `car_spawn`), LOADING
camera, intro and countdown, RACING. In RACING `race_phase_6_update` records time trial splits and asks `game_mode_end_check`
(`src/tier2/an_hk_689.c`, a switch over `game_mode`) whether the race is over; if so `race_check_winner_by_game_mode` either enters
the winner phases (`career_event_won`) or the standings via `race_set_phase(RACE_PHASE_STANDINGS)` and `hudpanel_reset_all`.
`race_restart` runs `race_phase_9_update` (resets, unequips weapons), resets the replay and goes back to phase 1.
The game modes (`GAME_MODE_*`, e.g. hold the flag, hare and hounds, beat the bomb) change the end rule and a few phase details.

## 3. Cars

One `Car` record per slot (16 slots, see STRUCTS.md), its visual twin is the `VehicleRecord`.

Per game tick, `cars_step` (`src/clusters/cl_cars_step.c`) is the physics driver: race time limit, ranking of the players, then
chunks of at most 50 ticks in which it calls `car_update` for every slot and sub-steps of at most 25 ticks of `cars_update_all`
(`src/clusters/cl_cars_update_all.c`).

- `car_update` (`src/clusters/cl_car_update.c`): per car and step. Works with the input bytes of `PlayerInput`; computes the sliding
  amount, the held times of accelerate and brake and the driving force of the first two wheels, then calls the three stages below. Mode 2
  slows the car by 10 percent per step.
  - `car_apply_forces` (`src/clusters/cl_car_apply_forces.c`): sums forces, gravity when airborne, wheel load.
  - `car_update_drivetrain` (`src/clusters/cl_car_update_drivetrain.c`): gears and gear shift timer, drive force, surface colour effects (stop, slow, push, boost).
  - `car_air_control`: steering torque while airborne.
- `cars_update_all`: for every active car `car_integrate_step` (`src/clusters/cl_car_integrate_step.c`), `car_wheel_contact_update`
  (`src/clusters/cl_car_wheel_contact_update.c`), `car_world_collision_test` and `car_ground_contact_forces`
  (`src/clusters/cl_car_ground_contact_forces.c`), then every later car is tested with a bounding sphere and car vs car hits are
  resolved (`car_car_collision`, `car_collision_impulse`) until none is left. Last it swaps the matrix double buffer
  (`matrix_current` / `matrix_previous`).
- Stats and scoring: `player_stats_update` (`src/clusters/cl_player_stats_update.c`) fills `PlayerStatRecord` (turbo skills, kills,
  cleanest drive).
- AI: `ai_bot_update` (`src/tier2/an_hk_707.c`) runs the stage state machine per bot (`AiBotRecord`), `ai_update_speed_scaling`
  (`src/clusters/cl_ai_update_speed_scaling.c`) is the rubber band that sets `Car.speed_scale`. The route data is `AiData`
  (a 128 x 128 grid of tiles with danger bytes and 4 route sets, **inferred** reading).

## 4. Camera

`camera_update` (`src/clusters/cl_camera_update.c`) is per viewport: zoom (clamped to 10) selects tilt, height and distance; the eye
follows the target car at a horizontal distance with smoothing. Constants `SECOND_CAR_MAX_GAP` and `HARE_MIN_FOLLOW_DISTANCE`
tune the follow in the hare and hounds mode. `camera_chase_update` (`src/clusters/cl_camera_chase_update.c`) is the chase behaviour,
`camera_update_view_window` derives the view window from field of view and aspect. During the race phases the camera is scripted
(loading path, winner view, see section 2).

## 5. Course loading

1. `race_setup_get_course` / `race_load_course_0042fad0` pick the course entry of the current race setup slot (`CourseEntry`: name,
   id, 13 callbacks). `course_load_from_setup` opens the course and runs its scripts.
2. `course_script_run` (`src/tier2/an_hk_387.c`) runs the course Lua script `COURSE.LUA`; its functions (`Clump_Filename`, `Mts_Filename`,
   `Spline_Filename`, `Setup_Fog`, `Lap_Line`, ...) fill the `CourseFiles` (names), `CourseCounts` and `CourseLap` blocks.
3. `course_create_from_description` (`src/clusters/cl_course_create_from_description.c`) builds the world from them: texture
   dictionary, world BSP, collision BSPs, clumps, matrix lists (MTS), splines, UVA animation, fog and lights. The result is the
   `CourseWorld` record (loaded clump/world handles).
4. Per course callbacks (`course_<name>_cb0`, `_cb1`, ...; `COURSE_MTS_LIST` addresses the matrix lists) add course specific
   behaviour (animated objects, hazards, ambience). `course_load_time_report` / `course_load_time_log` (`src/course.c`) report the load time;
   `AppState` becomes `APP_STATE_COURSE_LOADED` and then the race.
5. Powerups are loaded by `powerup_scripts_load` and `powerups_load_assets` (`src/clusters/cl_powerups_load_assets.c`); `Powerup` records
   respawn on a timer.

## 6. Menus and frontend

The frontend is a token driven page system.

- `MenuRecord` (one per open page): `page_id` (`MENU_PAGE_*`, listed in ENUMS.md: `MENU_PAGE_PAUSE`, `MAIN`, `SINGLE_PLAYER`,
  `MULTIPLAYER`, `OPTIONS`, `SOUND`, `CONTROLLERS`, ...), a pointer to the item list, the selection and per-entry enabled flags.
- Item lists end with a terminator; items are tokens of the form `0xFFnn0000`. The handler constants are named
  `MENU_ITEM_*` (defined in `src/clusters/cl_menu_update.c`): for example `MENU_ITEM_START_RACE`, `MENU_ITEM_START_RACE_SETUP`
  (starts a race with phase SETUP), `MENU_ITEM_STANDARD_PLAY`, `MENU_ITEM_TEAM_PLAY`, `MENU_ITEM_TIME_TRIAL_STANDARD`,
  `MENU_ITEM_OPEN_OPTIONS`, `MENU_ITEM_OPEN_SOUND`, `MENU_ITEM_RESUME`, `MENU_ITEM_SAVE_GAME`, `MENU_ITEM_EXIT_TO_MAIN`,
  `MENU_ITEM_LOAD_COURSE`. Their meaning is **inferred** from what the handler does.
- `menu_update` (`src/clusters/cl_menu_update.c`) is the state machine: volume/flash fades, fade in/out of the page, and in state 3 the
  input handling (select runs a switch over the item token, back/up/down/left/right/next). `menu_page_enter` (name table only) opens a page,
  `menu_page_update_entry_states` (`src/tier2/an_hk_9432.c`) enables or disables entries from the career progress, player count and options.
- Item animation: `MenuItemAnim` with the states `MENU_ITEM_APPEARING`, `MENU_ITEM_IDLE`, `MENU_ITEM_DISAPPEARING`, `MENU_ITEM_FINISHED`
  (`menu_items_animate_update`, `src/clusters/cl_menu_items_animate_update.c`).
- Drawing: `frontend_draw` (`src/clusters/cl_frontend_draw.c`) renders states, panels and the message box; `menu_draw_*` functions draw
  the selection screens; `results_screen_draw` (`src/clusters/cl_results_screen_draw.c`) the race results; team colour panels
  are `TeamPanelEntry` records (`team_panel_reset`, `src/clusters/cl_team_panel_reset.c`).
- Persistent data is the `GameState` record (career progress grid, records, best times, option sliders).

## 7. Weapons and power-ups

- Pickups are `Powerup` records (state 1 available, 2 taken and respawning); a taken pickup gives the player a weapon of `WEAPON_*`
  (mortar, machine gun, depth charge, missile, mine, flame thrower, shotgun, flare, oil, chaos, airstrike).
- A player's weapon is a `WeaponSlot` pointing to an entry of the table `weapon_descriptors`; each descriptor has callbacks
  (equip, fire, update, render, race start, reset). The dispatchers are `weapons_update_frame` (`src/tier2/an_hk_1004.c`),
  `weapons_race_start_dispatch` (`src/clusters/cl_weapons_race_start_dispatch.c`), `weapons_effects_render_all` and
  `weapons_reset_all_dispatch`.
- Machine gun: `MachinegunState`. Drum (barrel) weapon: `weapon_drum_*` (`weapon_drum_drop`, `src/clusters/cl_weapon_drum_drop.c`).
- Rockets: `RocketRecord`; `rocket_fly_straight` (`src/clusters/cl_rocket_fly_straight.c`, sine wobble), `rocket_fly_homing`
  (`src/clusters/cl_rocket_fly_homing.c`, steering at a target car), `rocket_hit_callback` (`src/tier2/an_hk_1000.c`), `rocket_explode`
  (`src/clusters/cl_rocket_explode.c`).
- Explosions and damage: `explosion_queue_update` (`src/tier2/an_hk_970.c`), `ExplosionEntry` (damage falloff, push, frames),
  `explosion_trigger_callback` (`src/tier2/an_hk_9216.c`). `Trigger` records are overlap spheres in a grid that call back
  for powerups (type 7), rockets (0xB) and explosions (0x19).

## 8. Effects and particles

- `fx_update_effects` (`src/clusters/cl_fx_update_effects.c`): `fx_update_all`, then the effect object table (`FxSceneObject`,
  0xec bytes each; spin, health, timers).
- `explosion_spawn_fx` (`src/clusters/cl_explosion_spawn_fx.c`) creates the visuals of an explosion from an `FxSceneObject`.
- Particles: trails use `FxTrailParticle` records (0x48 bytes): `fx_trail_particle_spawn` (`src/tier2/an_hk_323.c`, rate limited by
  distance to the last emission point), `fx_trail_particle_spawn_interpolated`, `fx_trail_batches_update`
  (`src/clusters/cl_fx_trail_batches_update.c`, ages trails and writes 4-point batches with fading alpha). Dust: `fx_dust_create`
  (`src/clusters/cl_fx_dust_create.c`), `fx_dust_particle_spawn` (`src/clusters/cl_fx_dust_particle_spawn.c`). `particle_emit_textured` is the
  textured emitter. Damage smoke: `car_damage_smoke_*`.
- Decals and surfaces: `explosion_decal`, `fx_decal_stamp_matrix`.

## 9. Audio

- `sound_stuff_init` (name table only) sets the sound system up; sound effects are loaded from the SFX archive by `race_load_common_assets`.
- Per car engine sound: `sound_car_engine_start`, `sound_car_engine_update` (`src/tier2/an_hk_773.c`); ambience per course:
  `sound_ambience_course_*` (name table only). Voices are positioned only while `AppState` is `APP_STATE_RACE`.
- Spoken lines (commentary) are queued: `voice_queue_frame_update` (per frame, name table only), `voice_queue_add`, `voice_queue_race_update`.
- The 3D listener is a DirectSound3D style record, `Listener3D` (**inferred** from its accessors in the sound library: position, velocity,
  front and top vectors, distance/doppler/rolloff factors and a dirty mask `LISTENER3D_DIRTY_*`).

## 10. Shared constants

`mfl_tier2.h` carries names that many sources use (guard `MFL_SHARED_BITS`):

- `CAR_ID_BASE` (0xff00): collider type and event parameter `CAR_ID_BASE + n` denotes car `n`.
- `VOICE_OFF_HANDLE`, `VOICE_OFF_FLAGS`, `VOICE_OFF_FREQ`, `VOICE_OFF_POS_X/Y/Z`: offsets inside a voice record; `VOICE_DEFAULT_FREQ_BITS` is the bit pattern of the default frequency (22050.0f).
- `MESSAGE_CODE_FF46`: the message code (second argument of message_show, stored in g_message_code; not a colour). `COLOUR_WHITE_ARGB`: white (all channels 0xff) colour argument.
- `FLOAT_ONE_BITS`: bits of 1.0f stored through an int-typed slot.

Generic small numbers (0x10, 0x20, ...) are strides of different structs and are deliberately not shared.
