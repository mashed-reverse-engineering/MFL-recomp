# Video interface (replacement for the statically linked DirectShow)

Status: design only (2026-10-29). `recomp/mfl_video.h` is not part of any build; no behaviour change.
Decision (user, 2026-10-29): DirectShow is replaced by a small platform independent interface; FFmpeg and libVLC are
later backends. On Windows x86 the original DirectShow code can stay as one backend (or a shim over the original bytes).

## What the DirectShow module is
Library code, not game logic (ROADMAP 2026-10-22): Microsoft strmbase baseclasses + Criterion "RwaDS" movie filters at
0x4ac600..0x4b0b70 and 0x5b5eb0..0x5d09d0 (about 58 of the unreplaced functions, C++ EH frames, `ecx` this, vtable calls).
Names/evidence: `names/MFL_strmbase_hints.csv`. None of it needs to be recompiled for other platforms; only the thin
application layer below talks to it.

## Application layer (the only call sites to map)
All of it lives at 0x4a3490..0x4a4060 + `video_play_loop` (0x4a44a0). The game plays ONE movie at a time and keeps
its state in globals: `g_app_76e8a4` (RwRaster the frame is rendered into), `g_app_76e890` (movie active),
`g_app_76e894` (keep-active flag: do not clear 76e890 at end), `g_movie_renderer_76e8a8` (movie id),
`g_movie_renderer_76e8b0/b4/b8/bc/c0`, `g_app_76e8ac` (COM interfaces: renderer filter, IMediaEvent, video window/renderer
control, filter graph, IMediaControl, IBasicAudio), `g_d3dx_76e89c`/`g_app_76e8a0` (D3D texture/surface of the renderer).

| Existing function (addr, state) | What it does today | mfl_video call |
|---|---|---|
| `video_texture_setup` 004a3fe0 (+dup 004a4040), tier1 C | `RwRasterCreate(0,0,0,0x84)`, `movie_renderer_create(id)`, sets active flag/keep flag | create raster, then `mfl_video_open(path_of(id), 0)` + `mfl_video_start` |
| `movie_renderer_create` 004a3a20, cluster C (EH root, partly original) | CoCreate FilterGraph, construct RwaDS texture renderer, AddFilter "TEXTURERENDERER", AddSourceFilter(path from table `PTR_s_toastart_pc_movies_*` @0062ad3c), Render pin, QI media control/event/audio, `Run`; error message boxes | `mfl_video_open` (error -> same message box text) + `mfl_video_start` |
| `movie_renderer_poll` 004a37f0, automatch C | `IMediaEvent::GetEvent(0)`; on EC_COMPLETE (1): stop renderer, clear active unless keep flag | `mfl_video_update`; `if (mfl_video_finished(v) && !keep) active = 0` |
| `movie_poll_and_set_surface` 004a39e0 (+dup 004a4050), tier1 C | poll, get raster of renderer buffer `[g_app_62ad50]`, `rw_set_value_at_root_plus_947144(raster)`, returns 1 | `mfl_video_update` + `mfl_video_acquire_frame`, copy/upload pixels into the raster (locked RwRaster), return 1 |
| `video_play_loop` 004a44a0, cluster C | 4 intro movies (ids 1..4): setup, loop {poll, clear teal, `video_draw_textured_quad` with ratio = video size / texture size, show raster, Sleep(5)}, skip on menu_select release, `app_get_app_76e890_3490` = still playing | `mfl_video_get_size` for the ratios (replaces `app_get_two_size_pairs`), `mfl_video_finished` replaces the active test; unchanged otherwise |
| `app_get_two_size_pairs` 004a34b0 | video size / texture size pairs | `mfl_video_get_size` + texture size known by the raster |
| `app_get_app_76e8a4` 004a34f0 | returns the renderer raster/texture | the game's own RwRaster (backend independent) |
| `app_get_app_76e890_3490` / `_4030` | movie-active getter (play loop, music fade, shutdown) | stays a game flag, set by `video_texture_setup`, cleared on finished |
| `movie_stop` 004a39a0 (+dup `movie_stop_b` 004a4060), tier1 C | 2 vtable calls on IBasicAudio-ish `g_app_76e8ac` (0xffffec78, 0xffffd8f0 = volume -50.00 dB, then -100.00 dB), `movie_renderer_release`, `RwRasterDestroy`, clear flags | `mfl_video_set_volume(v,0)`, `mfl_video_close(v)`, `RwRasterDestroy` |
| `movie_renderer_release` 004a3860, tier1 C | Release x7 COM interfaces + D3DX/app objects | `mfl_video_close` (inside) |
| `movie_renderer_delete` 004a3910, `movie_renderer_construct/destruct` 004a30b0/3110, `movie_guid_compare` 004a3155 | C++ object plumbing of the RwaDS texture renderer | disappear (backend internal) |
| `sound_volume_to_directsound_db` 004a3930, cluster C | linear 0..1 -> DirectSound hundredth-dB; called from `sound_music_fade_step_b` only while a movie is active | `mfl_video_set_volume(v, linear)`; the dB formula moves into the DirectShow backend |
| `app_shutdown_resources` (tier1) | if movie active: `movie_stop_b(0)` | `mfl_video_close` through `movie_stop` |
| `movie_texture_open/release` 00403f20/40 (batch4.c) | NOT video playback: caps probe + raster for the menu "movie" background texture (`video_check_texture_caps`) | out of scope, unchanged |
| `movie_draw_fullscreen_quad` / `movie_present_frame` 00403f80 / 004041f0 | name only; they draw a quad of the frontend texture through `gfx_draw_movie_rect_2d` (483510) | consumer of the raster, unchanged |

`video_draw_textured_quad` (004a3500, D3D) is the consumer of the frame and is not part of the interface.

## Frame transfer
Original: the RwaDS filter renders into a D3D texture; the game wraps it in an RwRaster (`app_get_app_76e8a4`).
New: backend returns BGRA pixels (`mfl_video_acquire_frame`); the game locks the raster and copies (or the backend
renders straight to a texture on GPU-capable backends later). The interface keeps the pixels path first because it is
the same for FFmpeg and libVLC (`libvlc_video_set_callbacks`).

## Behaviour notes
- Movies: `toastart\pc\movies\{intro,renderware,supersonic,empire,frontend}.mpg` (table at 0062ad3c; MPEG-1).
  An FFmpeg backend needs mpeg1video + mp2 audio and its own audio output (the original renders audio through DirectShow).
- Audio: the interface owns movie audio. Music is faded by the game (`sound_music_fade_step`) while a movie plays.
- Finished flag mirrors `EC_COMPLETE` only; errors at open are reported once by the game (original message boxes).
- Hybrid/Windows builds keep the original bytes, so the interface is only needed for non-Windows or non-DirectShow builds.
  The 5 overlay stubs (`tools/wine-run/prepare.py` writes `0xC3` into movie_stop, movie_renderer_release, movie_renderer_create,
  video_play_loop, sound_volume_to_directsound_db for the Wine harness) show where the seam already is.

## Migration order (later, not now)
1. Add backend `directshow` = the current code behind the interface (no behaviour change, verify with the existing gates).
2. Rewrite the six game functions above to call `mfl_video_*` (diffemu will no longer apply: they stop being byte/semantics clones; gate by sweeps).
3. FFmpeg backend, then libVLC.
