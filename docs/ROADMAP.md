# Roadmap: milestones and open work

State as of 2026-11-17. Each item is marked **done**, **in progress** or **planned**. No dates are promised; order inside a group is the intended order. Switches for behaviour changes are listed in [MFL_FIX_FLAGS.md](MFL_FIX_FLAGS.md). Numbers per layer: [PROGRESS_HISTORY.md](PROGRESS_HISTORY.md).

## Milestones reached

| Milestone | State |
|---|---|
| Name functions, globals and structs of the game code | **done** for about 96% of the game functions |
| Recompile the game code as C (byte identical or behaviour verified) | **done** for about 98% of the game functions; the rest are tiny stubs |
| Standalone executable from the C, with Lua, qhull and zlib/libpng from open source, no DirectShow | **done**; passes the 15-course regression runs |
| C runtime start-up and code from libcmt | **done** for code |
| RenderWare physics plugin as verified C | **done** |
| RenderWare and D3DX replaced by the clean-room `librw-mfl` | **done** for code |
| Audio library replaced (output behind a small audio interface, DirectShow path removed) | **done** for code |
| Video: small video interface; FFmpeg backend as a separate DLL | **done** for the interface; the FFmpeg backend plays the intro and menu movies under Wine in our tests and is not shipped here |
| Zero original code in the build | **done** 2026-11-15 |
| Zero original resources in the build (read at runtime from your `MFL.exe`) | **done** |
| Library variables (.bss) as named C definitions | **done** |
| Drop-in build script `tools/build_mflng.sh` | **done** in the maintainers' layout; see the self-contained build below |

## 1. Copied data to 0 B, and the self-contained build

| Item | State |
|---|---|
| Library data tables as typed C or removed as unused | **in progress**: 5,874 B left (C runtime 2,422 B, RenderWare 948 B, Lua 657 B, qhull 432 B, 1,408 B of gaps, 7 B without an owner layer); was 134,233 B on 2026-11-15 |
| Every "unused" data row proven by four checks (pointer closure, absolute-word scan of the linked exe, name scan over `.c` and `.inc` files, extents of the C objects in front) | **in progress**, a rule for every removal batch |
| Build from this repository alone: the files that still include a decompiler header (135 `ag_*` files and two bridge files) move to the curated headers | **in progress** |
| `tools/build_mflng.sh` from an export tree, in a clean container with an empty object cache and a different mount path; result byte-identical to the gate's release link except the timestamp | **in progress** |
| Self-contained CI build on a public runner (needs copied data at 0 B and a compiler that CI may use) | **planned** |
| A second compiler path (clang-cl, clang, gcc) next to the matching MSVC build; the MSVC build stays the reference for byte matching | **planned**; needed for the Switch port anyway |

## 2. Behaviour: determinism and replay

The aim is that recorded input replays give the same game state frame by frame, so a refactor can be checked over a whole race.

| Item | State |
|---|---|
| Cause of the run-to-run difference of the original: sound code draws from the same random generator as gameplay, and audio timing moves those draws | **done** (explained); the fix is `MFL_FIX_DETERMINISTIC_AUDIO_RNG`, default off |
| State hashing per frame, compare of two runs, named classes of differences (sound words, uninitialised stack reads of the original) | **done** as tools; nothing is masked silently |
| First remaining difference between a run of ours and a run of the original: car matrix floats on course 0 (race frame 143) and course 7 | **in progress**: reads of memory the hash does not cover and the x87 control word at the diverging call are checked first |
| Compare of x87 operation order between our code and the original, as a check for float-order defects | **in progress** |
| State-hash step in the regression gate, report only | **in progress** |
| Input-only replay over a whole race | **planned**; starts when a whole race on courses 0 and 7 is identical. Until then it is accepted on two runs of the original only |
| Open decision: the original reads some stack memory it never wrote; identity with ours is not reachable unless these words are zero-initialised or masked. Default: no change, the words stay visible | **open** |
| More in-game test scenarios (every game mode, 1 to 4 players, weapons and power-ups, pause and resume, long sessions) and a coverage report of functions no test reaches | **in progress** |

## 3. Readability

| Item | State |
|---|---|
| Names for functions, locals and struct fields (placeholder names do not count) | **in progress** |
| Module layout: code moves from folders that record how a function was matched (tier 1, automatch, clusters, bridges) into game modules (race, cars, weapons, frontend, camera, audio, video, input, platform) | **planned** |
| Drop provenance markers, copies and aliases that only serve the matching tools | **planned** |
| Plain calling conventions in place of register-argument clusters and naked entry stubs (breaks the per-function compare against the original, so it needs a replay-based test first) | **planned** |
| Direct calls instead of calls through the entry jump at the original address | **planned**, needs its own proof |
| Unpin the data layout from original addresses (also needed for ARM) | **planned** |
| Retire the matching scaffolding: trap and dead-code rows, allow lists, the overlay and harness build paths that exist only for matching | **planned** |
| Game data as new typed C files (resources stay external) | **in progress**, see section 1 |

## 4. Bug fixes behind switches

Every behaviour change is behind a default-off `MFL_FIX_*` switch; the default build is byte-identical to the original behaviour. See [MFL_FIX_FLAGS.md](MFL_FIX_FLAGS.md).

| Item | State |
|---|---|
| Tyre / trail strip refuses NaN and infinite points (`MFL_FIX_TRAIL_NAN`) | **done** |
| Audio random numbers on their own stream (`MFL_FIX_DETERMINISTIC_AUDIO_RNG`) | **done** |
| Mines, drum, flashbang, shotgun: drop + equip in one frame writes past the 4-record equip list (`MFL_FIX_EQUIP_FLUSH`; trainer / debug only) | **in progress**: mines reproduced in the original and the fix verified under Wine; the other three by code read, waiting for the regression gate |
| Fixes for other original bugs found by the tests | **planned**, one switch each |

## 5. Quality of life

| Item | State |
|---|---|
| Widescreen: camera field-of-view factor from the back-buffer ratio (`MFL_FIX_WIDESCREEN`) | **planned**; first step is the camera factor only. HUD and menu placement come after |
| Controller input through SDL (`MFL_FIX_INPUT_SDL`): hot-plug, PlayStation, Xbox and Switch pads, Linux and Switch; DirectInput stays the default | **planned** |
| Rumble from the game's existing rumble requests (no force-feedback call reaches the pad on PC today) | **planned** |
| Analog triggers for gas and brake, with a "classic" (digital) and a "modern" default layout | **planned**; first check whether the physics accepts a throttle amount |
| In-game controls menu, binding and reassignment while playing | **planned** |
| In-game settings menu in place of the Windows dialogs (also needed on the Switch) | **planned**; a design exists |
| Fixed-step simulation decoupled from rendering, then render interpolation above 60 fps | **planned**; the game simulates at a fixed 60 Hz locked to rendered frames |
| Gyro steering, mid-game pad reassignment | **planned** |
| Weapon rules, trainer options as game settings (drive over revive, auto equip, destroy distance), camera variants, first-person camera | **planned** |
| Per-player voice language, more than 4 players (the game is hard-coded to 4 almost everywhere), LAN / online play, split-screen with per-player cameras | **planned**, large |
| Windowed / borderless mode, volume sliders, skip intro videos, asset override folder, crash handler | **planned** |

## 6. Renderers and graphics

| Item | State |
|---|---|
| Backend-neutral render-state layer in `librw-mfl` for the game's own direct Direct3D 9 calls | **in progress**: design written, no code until it has been read and agreed |
| OpenGL 3 / GLES backend (from upstream librw) | **planned** |
| Vulkan or other backends | **planned** |
| Early Linux build through dxvk-native (Direct3D 9 on Vulkan) | **planned**; not for the Switch |
| Graphics options: resolution and aspect, MSAA, anisotropic filtering, shadow maps, draw distance, per-pixel lighting | **planned** |
| Texture replacement hook: load `<hash>.png` overrides for community packs; no original textures shipped | **planned** |

## 7. AI

| Item | State |
|---|---|
| Report on the structure of the current AI and its tuning values | **planned** |
| Easy improvements to the existing AI (catch-up, route following, weapon timing, reaction delay, per-bot personality) | **planned**, behind `MFL_FIX_AI_*` |
| Classic game-AI techniques (utility AI, steering, overtaking, difficulty by perception limits) | **planned** |
| Learned behaviour from recorded human games, small networks, CPU inference, must run on the Switch | **planned** |

## 8. Recording and dataset

| Item | State |
|---|---|
| Game recording: per frame and player the input, device class, human or AI, a compact state snapshot; session header with build id, course, mode, settings, active switches, random seed | **planned** |
| Opt-in toggle, anonymous, files written locally, upload a separate manual step | **planned** |
| Open format, converter to a machine-learning format, a small viewer, dataset licence chosen up front | **planned** |

## 9. Ports

| Item | State |
|---|---|
| Linux build | **planned**: first through dxvk-native, later with a native renderer |
| Switch homebrew port (libnx, aarch64; no official SDK) | **planned**: needs the second compiler path, the SDL input backend, the in-game settings menu and a GL3 renderer |
