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
- `tools/build_mflng.sh` builds two executables: `MFLng.exe` (every switch off) and `MFLng_fixes.exe` (every switch on). `MFLng_fixes.exe` is **not gated**: no regression run checks it.
- There is no umbrella switch `MFL_FIX_ALL`. It will be added when two switches have to be tested together.
- A runtime configuration (an ini file or environment variables read at boot) is an option for later, behind its own switch; until then switches are compile-time only.

## Implemented

### MFL_FIX_TRAIL_NAN
- **Status:** implemented, default off.
- **Changes:** the tyre / trail strip refuses points that are NaN or infinite, instead of adding them to the strip.
- **Lives in:** `src/tier2/an_ok_055.c` (private tree: `automatch_near/an_ok_055.c`).
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_TRAIL_NAN=1`, or use `MFLng_fixes.exe`.

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
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_EQUIP_FLUSH=1`, or use `MFLng_fixes.exe`.

### MFL_FIX_WIDESCREEN
- **Status:** staged, default off.
- **Original problem:** the 3D camera always uses a 4:3 view, whatever video mode is chosen, so at a wide mode such as 1280x720 the picture is stretched sideways.
- **Change:** the camera aspect factor is taken from the video mode (height / width) instead of the fixed 0.75. A wide mode shows more of the course at the sides; the view height stays as in 4:3. At 4:3 modes nothing changes. Two size readers of the view window (pickup icons above the cars, a skid-mark value) are corrected so they keep their 4:3 size.
- **In-race HUD:** the HUD keeps its proportions; panels on the left or right (score, team, kill-count and endpoint panels) move out to the screen edges, centre items stay centred, player markers follow their cars over the whole width.
- **Menus:** the frontend pages, the options menu and the message boxes keep their 4:3 proportions, centred, with plain side bars where a full-screen menu background would have been stretched.
- **Not yet:** the in-race 640x480 2D passes (flash text, race messages, training boxes) and the menus drawn over a running race start (app states 5 / 6) are still stretched.
- **Lives in:** `camera_update`, `camera_apply_rotation` (with the helpers `mfl_widescreen_*`), `camera_path_update`, `hud_pickup_icons_update_render`, `vehicle_skidmarks_render`, `app_state_menu_render`, `score_panel_entry_update`, `team_panel_entry_update`, `kc_panel_entry_update`, `endpoint_panel_entry_update`, `hudpanel_marker_place`.
- **Turn on:** `MFL_FIX_DEFINES=/DMFL_FIX_WIDESCREEN=1`, or use `MFLng_fixes.exe`.

## Planned

| Switch | What it would change | Notes |
|---|---|---|
| `MFL_FIX_WIDESCREEN` step 2 | 2D HUD and menus keep their proportions instead of being stretched (the former `MFL_FIX_HUD_ASPECT`, now part of the same switch). | Design in the widescreen study; layout rule (centre or edges) still open. |
| `MFL_FIX_INPUT_SDL` | Controller input through an SDL game-controller backend (hot-plug, Xbox, PlayStation and Switch pads, Steam Input, rumble, later gyro) instead of DirectInput. | Part of a small `mfl_input` interface with the DirectInput backend as default. Opt-in on Windows; the default input path on Linux and Switch. |
| `MFL_FIX_REMAP_MENU` | An in-game page for controller bindings. | The original already reads `contcfg%d.bin` (0x200 bytes per controller), so an external tool can write bindings without a switch. |
| `MFL_FIX_SETTINGS_MENU` | In-game settings pages next to (later in place of) the Windows setup dialogs. | A design exists. |
| `MFL_FIX_VIDEOCFG_VALIDATE` | Clamp a bad video mode or adapter index loaded from a settings file. | Optional second switch of the settings menu design. |
| `MFL_FIX_FIXED_STEP` | The frame timing writes the real frame delta into the tick count instead of the constant 50 units, so several 1/60 s simulation steps can run per frame; the 16.67 ms sleep cap is raised. | Needs input edges polled inside the tick loop. Smooth play above 60 fps needs render interpolation as well. |
| `MFL_FIX_LOAD_*` (family) | Shorter load screens. | Load times are measured first; it is not known yet whether a fixed wait exists. |
| `MFL_FIX_AI_*` (family) | Opponent AI changes, one switch each. | The original AI stays the default. First step is a read-only report. |
| Graphics options (family, names not fixed) | Resolution and aspect, MSAA, anisotropic filtering, depth precision, shadow maps, per-pixel lighting, draw distance, render interpolation above 60 fps (the logic stays at a fixed 60 Hz). | Depends on the render-state layer in `librw-mfl`. |
| Recording option (name not fixed) | Per-frame recording of inputs and a compact state snapshot for a community dataset. Opt-in, anonymous, never changes gameplay. | May be a build option instead of a game switch. |
| `MFL_FIX_MINE_DROP_EMPTY` | `weapon_mine_drop` returns early when the equip record holds no mine, instead of reading a pointer at record-4. | Defence only; not reachable in the original flow. |
| `MFL_FIX_MINE_MATRIX_INIT` | Zero the local matrix before building the mine matrix. | Determinism only, not a crash fix. |
| `MFL_FIX_RUNTIME_CFG` | Read switch settings at boot from an ini file instead of compile time. | Only an option; the reader itself would sit behind this switch so the default build stays identical. |

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
