# MFL_FIX switches

Every change to the original game's behaviour sits behind a compile-time switch named `MFL_FIX_<NAME>`. All switches are **off by default**: the default build behaves like the original and is the build the regression gate checks. This file lists every switch, implemented or not, so that the list can be kept for the future.

Status words:
- **implemented**: in `src/mfl_fix.h`, in the gated tree.
- **staged**: written and verified, waiting for the regression gate; not in the tree yet.
- **planned**: named in the design notes, no code.

## How the mechanism works

- All switches are defined in one header, `mfl_fix.h` (three equal copies, one per build folder: `automatch_near`, `clusters`, `tier1`; `tools/fixlint.py` checks that they are equal). Each switch has a block like this:

  ```c
  #ifndef MFL_FIX_TRAIL_NAN
  #define MFL_FIX_TRAIL_NAN 0
  #endif
  ```
- Default is `0`: original behaviour. Turn a switch on with `/DMFL_FIX_<NAME>=1` on the compiler command line.
- Test a switch with `#if MFL_FIX_X`, never with `#ifdef`: the header defines every switch, so `#ifdef` would always be true.
- `tools/build_overlay.py` takes the defines from the environment variable `MFL_FIX_DEFINES`, for example `MFL_FIX_DEFINES=/DMFL_FIX_DETERMINISTIC_AUDIO_RNG=1`. The defines are part of the object cache key, so a switched object never replaces a default one.
- `tools/build_mflng.sh` builds two executables: `MFLng.nofix.exe` (every switch off) and `MFLng.exe` (every switch on). `MFLng.exe` is **not gated**: no regression run checks it.
- There is no umbrella switch `MFL_FIX_ALL`. It will be added when two switches have to be tested together.
- Bug fixes stay compile-time switches. Player preferences (things that are a matter of taste, not bugs) are keys of a settings file read at start, all behind the one switch `MFL_FIX_PREFS`.

## Implemented

### MFL_FIX_TRAIL_NAN
- **Status:** implemented, default off.
- **Changes:** the tyre / trail strip refuses points that are NaN or infinite, instead of adding them to the strip.
- **Lives in:** `src/tier2/an_ok_055.c` (private tree: `automatch_near/an_ok_055.c`).
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_TRAIL_NAN=1`, or use `MFLng.exe`.

### MFL_FIX_DETERMINISTIC_AUDIO_RNG
- **Status:** implemented, default off.
- **Changes:** the original game uses one random generator (RenderWare's `RpRandom`, seeded with a constant) for gameplay and for sound. When a voice clip ends one frame earlier or later, the sound code takes its random numbers one frame earlier or later, and every gameplay draw after that moves along the sequence. With the switch on, the sound code draws from its own generator stream (same algorithm, own seed); gameplay keeps the original sequence. What you hear can still change with audio timing, but not what happens in the race. This is needed for input-only replays without pinning the audio.
- **Lives in:** `src/tier1/mfl_audio_rng.h`, `src/tier1/random_range.c`, and the switch block in `mfl_fix.h` (the files of the sound functions with random-draw sites define `MFL_AUDIO_RNG_SITES` before including the header; with the switch on, their calls to the random wrappers go to the audio-stream copies).
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_DETERMINISTIC_AUDIO_RNG=1`.
- **How it is tested:** the original cannot be built with the switch, so the comparison control runs the original with a test shim that patches the same 102 sound draw sites (22 functions) to a separate stream (`SPLIT=1` in the trace harness); our build gets the split from the switch. Gameplay words must match between the two; sound-timing words are reported on a separate line.
- **Known side effect:** the shared gameplay stream no longer holds the draws the original's sound code made, so later gameplay draws are different numbers than in an original run that was not split. Race physics and car state stay identical in the course-0 comparison (374 frames). The spark difference once seen at the first contact on course 0 (race frame 224) was not caused by the split: it was a stack-layout bug in the collision event record, fixed without a switch; the shared stream matches the split control draw for draw.

## Staged (waiting for the regression gate)

### MFL_FIX_EQUIP_FLUSH
- **Status:** staged, default off.
- **Original bug:** four weapons keep their equip records in a shared list of 4: mines, drum (depth charge), flashbang and shotgun. A drop only marks the record for release; the release runs later in the per-frame weapon update. If three or four players hold the same one of these weapons and each is dropped and re-equipped before that update runs, the equip search finds no free record, falls through with index 4 and writes a fifth record past the list: mines over the mine model templates, drum and flashbang over the neighbouring weapon data, shotgun over the status words of players 2 and 3. A following call into `RpWorldAddClump` can then crash. Normal play cannot reach the bug: in a race the release always runs in the same frame as the drop, and a pickup never replaces a held weapon. Only an outside drop + equip (a trainer, or a debug menu) reaches it.
- **Change:** at the top of each of the four equip functions, every record of that weapon that is marked for release is released first, with the same call the per-frame update makes.
- **Verified:** mines under Wine: with the switch on there is no write to the fifth record and no crash; in the original the write happens and a page fault follows.
- **Lives in:** `weapon_mines_equip`, `weapon_flashbang_equip` (`src/tier2/an_ok_044.c`), `weapon_drum_equip` (`src/tier2/an_ok_042.c`), `weapon_shotgun_equip` (`src/tier2/an_ok_046.c`).
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_EQUIP_FLUSH=1`, or use `MFLng.exe`.

### MFL_FIX_WIDESCREEN
- **Status:** staged, default off.
- **Original problem:** the 3D camera always uses a 4:3 view, whatever video mode is chosen, so at a wide mode such as 1280x720 the picture is stretched sideways.
- **Change:** the camera aspect factor is taken from the video mode (height / width) instead of the fixed 0.75. A wide mode shows more of the course at the sides; the view height stays as in 4:3. At 4:3 modes nothing changes. Two size readers of the view window (pickup icons above the cars, a skid-mark value) are corrected so they keep their 4:3 size.
- **In-race HUD:** the HUD keeps its proportions; panels on the left or right (score, team, kill-count and endpoint panels) move out to the screen edges, centre items stay centred, player markers follow their cars over the whole width.
- **Menus:** the page backgrounds (the menu backdrop with its pictures and wave grid, the header and footer strips, the fade overlay) fill the whole width, stretched; the text and the menu elements (buttons, rows, icons, cursors, the race-results replay window) and the message boxes keep their 4:3 proportions, centred. The same holds for the pause page and the results page.
- **Over the race:** the in-race 640x480 2D passes (flash text, race messages, intro / winner banners, training boxes, bomb timer text) are drawn 4:3 in the middle the same way; the black banner bars stay full width. The player-marker pass uses the HUD's widened window, so the markers keep their shape over their cars.
- **Not yet:** movies; split-screen is not checked.
- **Lives in:** `camera_update`, `camera_apply_rotation` (with the helpers `mfl_widescreen_*`), `camera_path_update`, `hud_pickup_icons_update_render`, `vehicle_skidmarks_render`, `app_state_menu_render`, `frontend_draw`, `credits_update_draw`, `menu_fade_overlay_draw`, `ui_camera_update_view_rect`, `score_panel_entry_update`, `team_panel_entry_update`, `kc_panel_entry_update`, `endpoint_panel_entry_update`, `hudpanel_marker_place`, `hudpanel_markers_render_all`, `viewport_set_view_window_080`, `viewport_set_view_window_game_mode_9`, `race_status_messages_draw`.
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_WIDESCREEN=1`, or use `MFLng.exe`.

### MFL_FIX_UNLOCK_ALL
- **Status:** staged, default off. A test / cheat switch: **not** part of `MFLng.exe` (its define line in `mfl_fix.h` ends in a comment, and `build_mflng.sh` turns on only the lines `#define MFL_FIX_<NAME> 0`). For players it is the key `unlock_all` of the settings file (`MFL_FIX_PREFS`, below); the compile switch forces it on whatever the key says.
- **Changes:** everything that is locked at the start is open: the Bonus Features entry of the main menu, the three tournaments, every career event, course and arcade mode. The game already has a hidden code for this (a direction sequence on the attract screen) that sets all 270 career progress cells to "done" and the bonus menu flag; the switch sets the same state after the default settings are made at boot and after every save load.
- **Your save stays as it was:** when the game saves, the real progress cells and the real bonus flag are written, not the forced ones. Career progress won while the switch is on is therefore not saved; options, records and best times are.
- **Lives in:** `gamesave_apply` with the helpers `mfl_unlock_all_force` / `mfl_unlock_all_unforce` (`src/tier2/an_hk_9109.c`), `game_options_reset` (`src/tier2/an_hk_788.c`), `gamesave_gather` (`src/tier2/an_hk_967.c`).
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_UNLOCK_ALL=1`.
- **Test harness:** scenario runs get the same state on both sides (original and ours) without the switch: `SCEN_UNLOCK=1` makes the harness write it in every front-end frame (scenario `unlock_all`).

### MFL_FIX_PREFS
- **Status:** staged, default off; on in `MFLng.exe`.
- **Changes:** nothing by itself. With the switch on, the game reads `mflng_settings.cfg` from the folder of the exe once at start (no command line). Each line is `key=value` with the value `0` or `1`; unknown keys, comment lines (`#` or `;`) and other values are ignored. Every key's default is the original behaviour except `display_mode` (default borderless), so without the file (or with an empty one) the game plays like the original in a borderless window.
- **Keys:**

  | Key | Default | Other value |
  |---|---|---|
  | `skip_intro` | `0` | `1`: the intro movies at start are not played. |
  | `skip_save_wait` | `0` | `1`: the "saving" / "loading" message box closes as soon as the save or load is done, instead of staying at least one second (manual save, autosave, load). |
  | `unlock_all` | `0` | `1`: everything is unlocked, as `MFL_FIX_UNLOCK_ALL` above (your save keeps its real progress; career progress won meanwhile is not saved). |
  | `winner_cam_rotation` | `1` | `0`: at the end of a round the camera still moves in to the winning car but does not circle it. |
  | `display_mode` | `borderless` (`2`) | `fullscreen` (`0`): exclusive full screen at `resolution` (not listed by the display: the desktop size, else the mode of the setup dialog / `videocfg.bin`). `windowed` (`1`): a framed window with a client area of `resolution` (made smaller when the frame would not fit the screen), centred. `borderless`: a frameless window over the whole primary desktop at the desktop resolution, no mode switch (Alt+Tab is instant). |
  | `resolution` | `auto` (the desktop size) | `WxH`, e.g. `1280x720`: the size for `fullscreen` and `windowed`; `borderless` ignores it (the Preferences row is greyed out and shows "Desktop" there). The Preferences page (Graphics > Resolution) offers Desktop and every full-screen mode of the display with 32 bit colour, at least 640 x 480 and 4:3 or wider. If the device refuses the mode at start, the game opens a 640 x 480 window instead (logged in `mashed.log`). |
  | `language` | `auto` | Read only with `MFL_FIX_LANG` (below). `0` English, `1` French, `2` German, `3` Spanish, `4` Italian, `5` USA. |
  | `setup_dialogs` | `0` | Read only with `MFL_FIX_LANG`. `1`: a start without options shows the video and controller setup dialogs again. |
  | `skip_flyover` | `0` | `1`: a race starts without the course fly-over camera; the countdown begins at once (the same end as pressing Return during the original's camera flight to the car). |
  | `menu_speed` | `1` | `2` or `4`: the menus outside a race (page changes, fades, sliding buttons, the save message timer) run that many times faster. The pause menu in a race keeps its speed. |
  | `fps_cap` | `60` | Read only with `MFL_FIX_FIXED_STEP` (opt-in, not in `MFLng.exe`) and `MFL_FIX_FPS_CAP`. `30`, or `0` = no limit (vsync or the graphics card sets the rate). The game speed stays the same at every frame rate. |
  | `vsync` | `on` | `off`: the picture does not wait for the screen refresh (tearing possible). Applies at the next start. |
  | `pad_deadzone` | `35` | Used only with `MFL_FIX_CONTROLS` (below). `5`, `10`, ... `50`: how far a gamepad stick must move before it counts, in percent of its travel, for all gamepads. `35` is the original's value. Below about 13 the setting has no further effect, because the game reads the stick in 16 steps per axis. In the game: Preferences > Controls > Direction keys > *Stick dead zone*, applies at once. |
  | `mute_unfocused` | `0` | `1`: the game sound and the menu music are silent while another window has the focus (also in exclusive full screen), with a short fade out and in. In the game: Preferences, Other, *Mute in background*, applies at once. |

  The key `controller_setup` (one start with the original controller setup dialog) is gone: the Controls category below does its job in the game. A line with it in an existing file is kept and ignored. `MFLng.exe -CS1` still shows the original dialog and then exits, as the original launcher's "Setup Controllers" button does.

  **In the game:** Options, first row *Preferences* (main menu and pause menu), then a category: *Graphics* (Display mode: fullscreen / windowed / borderless; Resolution once the video code offers its mode list; Frame rate limit `fps_cap` only in a build with `MFL_FIX_FIXED_STEP`, applies at once; Vsync `vsync`, next start), *Comfort* (Intro movies `skip_intro`, Course flyover `skip_flyover`, Winner camera spin `winner_cam_rotation`, Menu speed `menu_speed`), *Cheats* (Unlock all `unlock_all`), *Other* (Language, auto or one of the six whose speech is installed, only with `MFL_FIX_LANG`; Wait after saving `skip_save_wait`; Mute in background `mute_unfocused`). Left / Right change the selected row; the values stand right of the page's widest caption; a box on the right explains the selected row and says when it applies, and shows "Restart the game to apply." while a changed row waits for the next start; as on the game's own option pages, Return on a value row accepts: it writes the file (lines that are not one of these keys stay as they were) and goes back one level; Escape goes back one level and gives back the values the page had when it was opened (also the ones that applied at once). On the Buttons and Direction keys pages a captured key is kept at once (Escape there restores only the dead zone); Reset on Controls is kept too. Display, resolution, language, intro movies and unlock all apply at the next start, the others at once. The page's own texts are ours (English, French, German, Spanish, Italian; the On / Off values are the game's texts); the game's text files are not changed.

  **Performance** (the options of the original video setup dialog, reachable even with `setup_dialogs=0`): *Dynamic shadows* and *Multi-texturing* (next start), *Trilinear filtering* and *Environmental effects* (next race). They are stored in `videocfg.bin`, not in `mflng_settings.cfg`, and written with the game's own `video_cfg_save`; their captions are the original dialog's strings, read from your `MFL.exe` (in the game's six languages). A start without `videocfg.bin` uses the original dialog's defaults (all four on). *Display adapter* in Graphics appears only when the video code lists more than one adapter (next start); choosing one also selects that adapter's first plain 32-bit full-screen mode of at least 640 x 480. A `videocfg.bin` with an out-of-range adapter or mode is treated as absent.

  Example file:
  ```
  skip_intro=1
  skip_save_wait=1
  winner_cam_rotation=0
  ```
- **Not yet:** faster loading (measured: a course loads in 0.3 s, nothing to gain); a menu speed row.
- **Lives in:** the reader `mfl_pref` and the writer `mfl_settings_save` in `src/mfl_resources.c`; the page in `src/mfl_settings_menu.c` (hooks: `menu_update`, `frontend_draw`, `hud_menu_draw_all_panels`, `dat_get_string`); `race_phase_3_update` (skip_flyover); `app_boot_init` (skip_intro); `gamesave_update_sub_40bbd0`, `gamesave_autosave_state_machine` and the load / format state machine in `src/tier2/an_hk_907.c` (skip_save_wait); `mfl_unlock_all_force` (unlock_all); `cup_scene_update_object` (winner_cam_rotation); `video_config_dialog` (display_mode, resolution, the Resolution row's mode list `mfl_video_modes_api`); `rw_engine_init` (start fallback).
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_PREFS=1`, or use `MFLng.exe`.
- **Test harness:** `SCEN_PREFS=<file>` (scenario.sh, standalone runs) puts the file beside our exe; the control never has one.

### MFL_FIX_LANG
- **Status:** staged, default off; on in `MFLng.exe`.
- **Original:** `launch.exe` starts the game as `MFL.exe -VS0 -CS0 -L<n>`, n = the language the installer stored in the registry (`HKLM\Software\Empire Interactive\MFL`, value `language`); its "Setup Video Mode" / "Setup Controllers" buttons start `-CS0 -VS1` / `-CS1 -VS0`. Started directly without options, the game shows both setup dialogs and runs in English.
- **Changes:** when the command line has no `-vs`, `-cs` or `-l` option, the game acts as if `-VS0 -CS0 -L<n>` had been given: no setup dialogs (the saved `videocfg.bin` / `contcfg<n>.bin` are used; without them the game's own defaults: the first full-screen mode of at least 640x480, which it then saves, and pads first, keyboard after), and n = the Windows display language when the game has speech for it (`toastaudio\pc\audio\pcdics\<language>` exists), else English. An option given on the command line always wins, so the launcher and the test harness behave as before. With `MFL_FIX_PREFS`, the keys `language` and `setup_dialogs` (above) override; the Preferences page sets `language` (applies at the next start).
- **Lives in:** `mfl_lang_apply` in `src/mfl_resources.c`, called from `app_initialise_on_bootup` right after `app_parse_command_line`.
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_LANG=1`, or use `MFLng.exe`.

### MFL_FIX_CONTROLS
- **Status:** staged, default off; on in `MFLng.exe`. Needs `MFL_FIX_PREFS`.
- **Original:** keys and buttons can only be changed in a Windows dialog before the game starts (launcher button "Setup Controllers", `-CS1`). It offers 5 actions on the keyboard and 6 on a pad, never the steering keys; "No Controller" is not saved; each player can only have the device the game assigns (pads first, then one keyboard); there is no dead zone setting. A saved profile is dropped when its device name differs from the assigned one, which also happens to every keyboard profile when the game language changes (the name is the translated word "Keyboard").
- **Changes:** Preferences gets a category *Controls* (the game's own text "Controllers" where it fits):
  - *Player* 1 to 4: the player the rows below show.
  - *Device*: none, the keyboard, or one of the gamepads found at start. A new device starts with the game's default keys or buttons; stepping back to the device the player had brings his keys back. The last device in use cannot be switched off.
  - *Buttons*: a page with the 9 button actions (accelerate, brake, fire weapon, drop weapon, insult, pause, menu select, menu back, menu next). Return on a row, then press the new key (keyboard player) or button (gamepad player); Escape cancels; after 10 seconds without a key the row is left as it was. If another action of the same group already used the key, that action gets the old key, so no action is ever without one. There are two groups: the race actions (accelerate, brake, fire, drop, insult, pause) and the three menu buttons. A key may be in both, as in the game's defaults (a pad's accelerate button is also menu select). Key names are the ones Windows gives in your keyboard layout.
  - *Direction keys*: the same for left, right, up and down of a keyboard player (a direction key shares with no other action). A gamepad always uses its stick or D-pad. The page also has *Stick dead zone* (the key `pad_deadzone`, above), and its box shows a cross of four squares that light up while the player's left / right / up / down are held, as the game reads them (a stick: after the dead zone); each square fills as far as its direction is pressed and shows that value in percent (a key: 0 or 100). The same cross is in the box of this row on the Controls page.
  - *Reset to defaults*: the device and keys the game gives that player without a file.
  - While a Preferences page is open (and, for the devices no player has, on the frontend pages where any player may steer: main, single / multiplayer, options and its pages, game set-up, challenge and arcade select, bonus, load / save; not on team colour, team / ability select, move screen, results or in a race), the menu also obeys the keyboard's default keys (arrows, Return, Escape) and the stick and default buttons (1 = select, 2 = back) of every gamepad found, whatever the players' devices and keys are. In the original the menus read only the players' own bindings, so a device nobody has is dead in the menu; without this the Device row could take the keyboard away from under the user's hands.
- **Applies:** at once, also in a running race (pause menu > Options > Preferences). Each change is written at once to that player's `contcfg<n>.bin` (n = 0..3) in the folder of the game's other files, with the game's own writer and in the original's format (0x200 bytes), so the original exe and its setup dialog read the same files.
- **Loading:** at start a saved file is kept when it has the right version, key codes in range and a device that exists: none, the keyboard (under any name and on any player), or a gamepad whose name is one of the gamepads found (the profile follows its gamepad when Windows lists them in another order). Players without such a file get the remaining devices in the original's order: gamepads, then the keyboard, then none. Without files this is exactly the original's assignment. The original exe keeps its stricter rule: where it rejects one of these files (a keyboard on a player other than the assigned one, a moved gamepad, "none") it uses its defaults for that player and leaves the file alone.
- **Limits:** gamepads are still found only at start (no hot-plug); two players can be given the same gamepad or the same keys (the page does not stop it); the texts that are ours exist in English, French, German, Spanish and Italian (the last four need a native speaker's check); a keyboard layout with letters outside Western European (Latin-1) may show wrong characters in key names. Tested under Wine with the keyboard and with simulated DirectInput gamepads; not yet on real Windows or with a real gamepad. Device and key names are cut where the description box begins. Outside the Preferences pages the menus still read only the players' own devices and keys.
- **Lives in:** `src/mfl_settings_menu.c` (rows, key capture `capture_run`, loader `mfl_controls_load`, menu input `mfl_controls_menu_input`), called from `controller_dialog_run` (`src/tier2/an_hk_999.c`) and `controller_poll_player`; the key in `src/mfl_resources.c`.
- **Turn on:** `MFL_FIX_DEFINES="/DMFL_FIX_PREFS=1 /DMFL_FIX_CONTROLS=1"`, or use `MFLng.exe`.

### MFL_FIX_PAD_LIMIT
- **Status:** staged, default off; on in `MFLng.exe`.
- **Bug:** at start the game takes every attached DirectInput game controller, however many there are, while its per-pad button and axis tables hold 4. A fifth device (virtual joysticks, wheels, pedals, flight stick parts, some keyboards and mice that register as a game controller) makes the per-frame poll write past those tables into the controller file path and the keyboard state. Found by reading the code.
- **Fix:** the enumeration stops after the fourth device (checked under Wine with five simulated devices: four are taken).
- **Lives in:** the enumeration callback `joypads_search_cb_4a5030`.
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_PAD_LIMIT=1`, or use `MFLng.exe`.

### MFL_FIX_ALT_TAB
- **Status:** staged, default off; on in `MFLng.exe`.
- **Bug:** after Alt+Tab (or the Windows key) out of full screen and back, the game looked hung and ignored the keyboard. Its window procedure counted the window as active only when `WM_ACTIVATE` carried exactly `WA_ACTIVE`; coming back to the minimised full-screen window (Alt+Tab: `WA_ACTIVE` plus the minimised flag; taskbar click: `WA_CLICKACTIVE`) it stayed in its background pause, which advances one frame per window message.
- **Fix:** any activation (low word of `wparam` not `WA_INACTIVE`) counts. The background pause and the keyboard release while another window is active stay as in the original; the lost Direct3D device is reset by the RenderWare code that was always there.
- **Lives in:** the window procedure `window_create_cb_4a9830`.
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_ALT_TAB=1`, or use `MFLng.exe`. If exclusive full screen still misbehaves after Alt+Tab, use `display_mode=borderless` (`MFL_FIX_PREFS`, the default of `MFLng.exe`).

### MFL_FIX_BACKGROUND_RUN
- **Status:** staged, default off; on in `MFLng.exe`.
- **Bug:** with the game in a window (windowed or borderless), switching to another window made the picture jump and the music slow down and break up, and again when coming back. While its window is inactive the original runs one frame per incoming window message (`WaitMessage` in its message pump): a few uneven frames, then a stop. The music stream is fed from the frame loop, so it starves.
- **Fix:** in a window the game keeps running while it is inactive: menus, loading and the music carry on evenly, so there is no stall to recover from when the window gets the focus back. The keyboard is still released while inactive. Two cases keep the original wait: a running race (so the race does not go on without the player) and exclusive full screen (the window is minimised there).
- **Not done:** a running race is held the original way, not by opening the pause menu, so its sound can still break up while the window is inactive.
- **Lives in:** the window procedure `window_create_cb_4a9830` (inactive in a window = state 2) and `app_main_loop` (the wait of a running race).
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_BACKGROUND_RUN=1`, or use `MFLng.exe`.

### MFL_FIX_USER_DIR
- **Status:** staged, default off; on in `MFLng.exe`.
- **Bug:** the game writes its files into its own folder. Installed where a normal user may not write (for example under `Program Files`), nothing is kept: settings, controller setup, the save game and the log are lost without a message.
- **Fix:** before a file is written, the game tries to open it for writing in its usual place. If that fails, the file goes to `%LOCALAPPDATA%\MFL` (the folder is made when first needed). A file that exists in `%LOCALAPPDATA%\MFL` is the one read and written from then on; without one, the game folder's file is read. A writable game folder behaves exactly as before.
- **Files:** `mashed.log`, `videocfg.bin`, `contcfg<n>.bin`, `gamesave.bin`, `mflng_settings.cfg`, and `mflng-crash.txt` (`MFL_FIX_CRASHLOG`: game folder, exe folder, `%LOCALAPPDATA%\MFL`, then `%TEMP%`).
- **Note:** to go back to the game folder's files after making it writable, delete the files in `%LOCALAPPDATA%\MFL`.
- **Lives in:** `mfl_user_path` in `recomp/mfl_resources.c`, called by `log_open_write`, `video_cfg_load`, `video_cfg_save`, `controller_cfg_load`, `controller_cfg_save`, `gamesave_load`, `gamesave_write`, `gamesave_exists` and the settings file reader and writer; `recomp/mfl_crashlog.c`.
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_USER_DIR=1`, or use `MFLng.exe`.

### MFL_FIX_LOAD_SAVE_EARLY
- **Status:** staged, default off; on in `MFLng.exe`.
- **Changes:** the game save is read and applied once at the end of `app_boot_init`, while the boot loading screen (its spinning CD) still shows. The load paths (title Load, and the return to the main menu) then show no confirm box, no "Loading" box with its 1 s wait and no "Load successful" box. The boot-time load only reads and applies the save; it never writes. The save side (write, autosave) is unchanged, and the `skip_save_wait` key of `MFL_FIX_PREFS` stays as it is.
- **Trade-off:** the original's confirm box with its "No" answer, which let the player decline the load (and so leave saving off), is gone.
- **Lives in:** `app_boot_init` (with the helper `mfl_load_save_at_boot`, `src/tier2/an_hk_9109.c`), the load state machine `gamesave_load_state_machine` and the load / format state machine in `src/tier2/an_hk_907.c`.
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_LOAD_SAVE_EARLY=1`, or use `MFLng.exe`.

### MFL_FIX_WINDOW_ICON
- **Status:** staged, default off; on in `MFLng.exe`.
- **Original:** the window class "DtS" is registered with no icon (`hIcon` is NULL), so the title bar, Alt+Tab and the taskbar show the system default.
- **Change:** the class icon is `LoadIconA(hinstance, 1)`: the exe's own icon group resource with id 1 (the CI build links `1 ICON "mflng.ico"` into the exe). Windows takes the small icon from the same group, so no separate small icon is set. An exe without that resource (the pinned VC7 link has none) makes `LoadIconA` return NULL, which is the original behaviour.
- **Lives in:** `window_create` (`src/tier1/window_create.c`; the switch-off code is the original statement, byte identical).
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_WINDOW_ICON=1`, or use `MFLng.exe`.

### MFL_FIX_WINDOW_TITLE
- **Status:** staged, default off; on in `MFLng.exe`.
- **Original:** the window title is "Drive to Survive [D3D9] [Release]".
- **Change:** the title is "Mashed: Fully Loaded - MFLng <tag>": the tag is the word after "release " on a line of `mflng\BUILDINFO.txt` in the exe's folder (the release build writes that file; never read from the working folder). Without the file or such a line (a local build) the title has no tag. A borderless window has no title bar: there the title shows only in the taskbar and Alt+Tab.
- **Lives in:** `mfl_window_title` (`src/mfl_resources.c`), called from `window_create` (`src/tier1/window_create.c`) right after the window is created.
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_WINDOW_TITLE=1`, or use `MFLng.exe`.

### MFL_FIX_CRASHLOG
- **Status:** staged, default off; on in `MFLng.exe`. Built only by MSVC v143 (the CI exe); `tools/build_overlay.py` leaves the file out of every other build.
- **What it changes:** nothing in the game. When the game ends abnormally (an unhandled exception, stack overflow, heap corruption, a CRT invalid-parameter stop, a pure virtual call, `abort()`, or `exit` / `ExitProcess` with a code other than 0), one report is appended to `mflng-crash.txt` in the folder where the game keeps `gamesave.bin` and `mashed.log` (the current folder; else the exe's folder, else `%TEMP%`). A normal quit writes nothing. The process then ends exactly as it would have without the switch, so Windows Error Reporting still sees the crash. The file is kept below 64 KB (one older copy: `mflng-crash.old.txt`).
- **Report:** time (UTC), the exe's time stamp (matches its `.map`), reason, exception code and address as module+offset (look the address up in `MFLng.map`), registers, stack words, return addresses, race / menu state, command line, Windows version.
- **Not caught:** `__fastfail` itself (Windows hands it straight to error reporting; see the Event Viewer).
- **Lives in:** `src/mfl_crashlog.c`.
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_CRASHLOG=1` with `CC=v143`, or use `MFLng.exe`.

### MFL_FIX_FPS_CAP
- **Status:** staged, default off; on in `MFLng.exe`.
- **Bug:** the game runs one 1/60 s simulation step per rendered frame, so the frame rate is the game speed. The main loop paced itself with one `Sleep` of whole milliseconds per frame; where `Sleep` oversleeps (Wine, a coarse Windows timer: the original raises the timer resolution only inside its movie player) the game ran at ~50 fps, i.e. ~84 % speed.
- **Fix:** the loop waits for the next point of a fixed 60 Hz schedule in 1 ms sleeps (timer period 1 ms). A late frame is made up by the next one; a stall of more than one frame restarts the schedule. The rate stays 60 and the simulation is unchanged; vsync stays on.
- **Lives in:** the main loop `app_main_loop`.
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_FPS_CAP=1`, or use `MFLng.exe`.

### MFL_FIX_FIXED_STEP
- **Status:** staged, default off, opt-in: NOT in `MFLng.exe` (like `MFL_FIX_UNLOCK_ALL`, its define line carries a comment, so the fixes build leaves it off). Build it yourself to try it.
- **What it changes:** the game speed no longer depends on the frame rate. The original runs exactly one 1/60 s simulation step per rendered frame (a slow frame is slow motion). With the switch each frame runs as many steps as real time has passed (rounded, the rest carried to the next frame), through the step loop the game already has. A frame runs at most 8 steps (below 7.5 fps the game slows down instead of falling further behind); a frame longer than 250 ms (a debugger stop), and the first frame after a race-phase or game-state change (a course load, a race start), count as one step, so a load does not trigger catch-up steps. At a steady 60 fps every frame is one step, as in the original.
- **Also per step:** button presses (a press during a frame without a step is kept for the next step, so a quick tap is neither lost nor seen twice), the sound frame (voice-queue timers), the game-time counter used by cameras and the race time.
- **With `MFL_FIX_FPS_CAP` and `MFL_FIX_PREFS`:** the key `fps_cap` (60 / 30 / 0 = no limit) sets the frame limiter. Above 60 fps the picture still moves in 1/60 s steps (frames repeat); smooth motion above 60 needs render interpolation (planned).
- **Lives in:** `app_frame_wait` (step count), `app_state_frontend_frame` and `tutorial_text_update` (steps with their input), `debug_keys_and_profile` (sound frame per step), `app_main_loop` (limiter).
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_FIXED_STEP=1` (with `/DMFL_FIX_FPS_CAP=1 /DMFL_FIX_PREFS=1` for the `fps_cap` key and the Frame rate limit row).

## Planned

| Switch | What it would change | Notes |
|---|---|---|
| `MFL_FIX_WIDESCREEN` step 2 | 2D HUD and menus keep their proportions instead of being stretched (the former `MFL_FIX_HUD_ASPECT`, now part of the same switch). | Design in the widescreen study; layout rule (centre or edges) still open. |
| `MFL_FIX_INPUT_SDL` | Controller input through an SDL game-controller backend (hot-plug, Xbox, PlayStation and Switch pads, Steam Input, rumble, later gyro) instead of DirectInput. | Part of a small `mfl_input` interface with the DirectInput backend as default. Opt-in on Windows; the default input path on Linux and Switch. |
| `MFL_FIX_PAD_PRECISION` | The stick is read at full precision instead of 16 steps per axis. | Changes the steering feel; opt-in until judged in a race. |
| `MFL_FIX_INPUT_XINPUT` | An XInput backend for Windows: triggers as buttons, Start as pause, rumble, hot-plug. | Needs a tester on real Windows with a gamepad. |
| `MFL_FIX_SETTINGS_MENU` | In-game settings pages next to (later in place of) the Windows setup dialogs. | A design exists. |
| `MFL_FIX_VIDEOCFG_VALIDATE` | Clamp a bad video mode or adapter index loaded from a settings file. | Optional second switch of the settings menu design. |
| `MFL_FIX_LOAD_*` (family) | Shorter load screens. | Load times are measured first; it is not known yet whether a fixed wait exists. |
| `MFL_FIX_AI_*` (family) | Opponent AI changes, one switch each. | The original AI stays the default. First step is a read-only report. |
| Graphics options (family, names not fixed) | Resolution and aspect, MSAA, anisotropic filtering, depth precision, shadow maps, per-pixel lighting, draw distance, render interpolation above 60 fps (the logic stays at a fixed 60 Hz). | Depends on the render-state layer in `librw-mfl`. |
| Recording option (name not fixed) | Per-frame recording of inputs and a compact state snapshot for a community dataset. Opt-in, anonymous, never changes gameplay. | May be a build option instead of a game switch. |
| `MFL_FIX_MINE_DROP_EMPTY` | `weapon_mine_drop` returns early when the equip record holds no mine, instead of reading a pointer at record-4. | Defence only; not reachable in the original flow. |
| `MFL_FIX_MINE_MATRIX_INIT` | Zero the local matrix before building the mine matrix. | Determinism only, not a crash fix. |

Not planned: a guard for the trainer's remote-thread races (a trainer problem, not a game bug).

## Adding a switch

One batch per switch:

1. Add its block to `mfl_fix.h`, with a one-line comment, in **all three copies** (`automatch_near`, `clusters`, `tier1`). Name: `MFL_FIX_<NAME>`, default `0`. No other prefix.
2. Test it with `#if MFL_FIX_<NAME>`, never `#ifdef`. A file that mentions a switch must include `mfl_fix.h`, and the name must appear in the header; `tools/fixlint.py` checks both, so a misspelt name cannot silently compile to "off". It must report 0 findings.
3. If you add a new header, add it to the build's local-header list.
4. A build with the switch on needs its `/D` in `MFL_FIX_DEFINES` (and so in the object cache key) in the same change.
5. **Off must be byte identical:** every file that includes the header compares SAME against HEAD (`tools/readable_vs_head.py`) and the linked executable is identical to the one before the change (`tools/pe_text_sha.py`, then the gate's link compare).
6. **On must build:** the flag-on build compiles and links with `RELCHECK=warn`.
7. **Wine check:** run the flag-on build once under Wine in the scenario that shows the change (a screenshot, or a log line), and read the trap log: it must be empty.
8. Add an entry to this file: name, status, what it changes, where it lives, how to turn it on.
