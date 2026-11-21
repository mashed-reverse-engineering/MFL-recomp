/* mfl_fix.h: every MFL_FIX_* switch. Default 0 = original behaviour. Turn one on with /DMFL_FIX_<NAME>=1.
   Test a switch with `#if MFL_FIX_X`, never #ifdef (this header defines every switch).  Copies: recomp/automatch_near/mfl_fix.h,
   recomp/clusters/mfl_fix.h, recomp/tier1/mfl_fix.h (equal; tools/fixlint.py checks).
   A flag-on build must put its /D into build_overlay.py's flag list (and so into the object cache key) in the same change:
   build_overlay.py takes them from MFL_FIX_DEFINES (e.g. MFL_FIX_DEFINES=/DMFL_FIX_DETERMINISTIC_AUDIO_RNG=1). */
#ifndef MFL_FIX_H
#define MFL_FIX_H

#ifndef MFL_FIX_TRAIL_NAN      /* trail strip refuses NaN / inf points (automatch_near/an_ok_055.c) */
#define MFL_FIX_TRAIL_NAN 0
#endif

#ifndef MFL_FIX_DETERMINISTIC_AUDIO_RNG      /* sound code draws from its own random stream, so audio timing cannot move gameplay random numbers
                                                (output/determinism_2026-11-16.md; tier1/mfl_audio_rng.h, tier1/random_range.c) */
#define MFL_FIX_DETERMINISTIC_AUDIO_RNG 0
#endif

#ifndef MFL_FIX_EQUIP_FLUSH      /* the equip functions of mines, drum, flashbang and shotgun first release their records whose release is pending,
                                    so a drop + equip in one frame cannot run past the 4 records (output/weapons_equip_sweep_2026-11-17.md;
                                    automatch_near/an_ok_042.c drum, an_ok_044.c mines + flashbang, an_ok_046.c shotgun) */
#define MFL_FIX_EQUIP_FLUSH 0
#endif

#ifndef MFL_FIX_WIDESCREEN      /* step 1: the 3D camera aspect factor is height / width of the video mode instead of the fixed 0.75 (4:3), so a wide
                                   mode shows more to the sides instead of a stretched picture; step 2: the in-race HUD keeps its proportions and its
                                   side panels move to the screen edges; step 3: menu text / elements, message boxes and the in-race 640-space 2D passes are drawn 4:3 in the middle, page backgrounds full width
                                   (output/widescreen_study_2026-11-17.md; automatch_near/an_hk_824.c holds the helpers) */
#define MFL_FIX_WIDESCREEN 0
#endif
#if MFL_FIX_WIDESCREEN      /* helpers in automatch_near/an_hk_824.c; step 2 = in-race HUD at the screen edges (void * so this header needs no RenderWare type) */
float mfl_widescreen_fov_factor(void);
void mfl_widescreen_hud_camera(void *camera);
float mfl_widescreen_hud_scale(void);
void *mfl_widescreen_hud_translate(void *matrix, const void *origin, int combine);
void mfl_widescreen_2d_rewiden(void *camera);      /* automatch_near/an_hk_611.c: menu pass camera window widened again after a reset */
void mfl_widescreen_2d_pass(void *camera, int im2d);      /* an_hk_611.c: an in-race 0.8-window 2D pass squeezed to 4:3 (im2d 0 = text only) */
void mfl_widescreen_2d_pass_end(void);
void mfl_widescreen_2d_full(int on);      /* an_hk_611.c: 1 = a page background inside a squeezed menu pass drawn full width, 0 = squeeze back */
#endif

#ifndef MFL_FIX_UNLOCK_ALL      /* hidden "100 %" unlock: after the boot defaults and after a save load the progress table and the bonus menu entry
                                   are set as the game's own attract-screen code sequence sets them (frontend_attract_menu_input); the save file
                                   keeps the real words (automatch_near/an_hk_9109.c helpers, an_hk_788.c, an_hk_967.c). Test / cheat switch: the
                                   define line ends in a comment so build_mflng.sh (lines `#define X 0` only) leaves it out of MFLng_fixes.exe */
#define MFL_FIX_UNLOCK_ALL 0      /* not in MFLng_fixes.exe; there it is the MFL_FIX_PREFS key unlock_all */
#endif
#if MFL_FIX_UNLOCK_ALL || MFL_FIX_PREFS      /* with MFL_FIX_PREFS alone the helpers act only when the cfg key unlock_all is 1 */
void mfl_unlock_all_force(void);
void mfl_unlock_all_unforce(void *save_block);
#endif

#ifndef MFL_FIX_PREFS      /* player preferences read at start from mflng_settings.cfg beside the exe (key=value lines, no command line); every key's
                              default is the original behaviour except display_mode (borderless, plan W17 F3) and resolution (desktop; this switch is on only in MFLng.exe) (recomp/mfl_resources.c reader;
                              keys and sites: public/docs/MFL_FIX_FLAGS.md); the in-game Preferences page under Options changes them (recomp/mfl_settings_menu.c) */
#define MFL_FIX_PREFS 0
#endif
#if MFL_FIX_PREFS
enum mfl_pref_key {
  MFL_PREF_SKIP_INTRO,            /* skip_intro=1: the intro movies are not played */
  MFL_PREF_SKIP_SAVE_WAIT,        /* skip_save_wait=1: save / load message boxes close when the write is done, not after 1 s */
  MFL_PREF_UNLOCK_ALL,            /* unlock_all=1: MFL_FIX_UNLOCK_ALL at run time */
  MFL_PREF_WINNER_CAM_ROTATION,   /* winner_cam_rotation=0: the round-end camera zooms to the winning car without circling it */
  MFL_PREF_LANGUAGE,              /* MFL_FIX_LANG: language=auto (-1, default) or a game language id 0..5 (see MFL_FIX_LANG) */
  MFL_PREF_SETUP_DIALOGS,         /* MFL_FIX_LANG: setup_dialogs=1: a start without options shows the video / controller dialogs (default 0) */
  MFL_PREF_DISPLAY_MODE,          /* display_mode=fullscreen|windowed|borderless (or 0/1/2): MFL_DISPLAY_* below; default borderless */
  MFL_PREF_SKIP_FLYOVER,          /* skip_flyover=1: race start without the course fly-over camera (race phase 3 ends in its first frame) */
  MFL_PREF_MENU_SPEED,            /* menu_speed=1|2|4: the frontend menus (not the pause menu in a race) animate that many times faster */
  MFL_PREF_RESOLUTION,            /* resolution=WxH or auto (-1, default = the desktop size); value (W << 16) | H; borderless ignores it.  The Resolution
                                     row sets it through mfl_video_modes_api->choose, so mfl_settings_save writes mfl_prefs[] for this key, not values[] */
  MFL_PREF_FPS_CAP,               /* MFL_FIX_FIXED_STEP: fps_cap=60|30|0 (0 = unlimited), kept as the index 0..2 of those words; default 60 */
  MFL_PREF_VSYNC,                 /* vsync=on|off (1/0): the flip waits for the vertical blank (the original) or not; applies at the next start */
  MFL_PREF_COUNT
};
int mfl_video_start_fallback(void);      /* rw_engine_init: RwEngineStart failed: once, a 640 x 480 window on the desktop mode (logged); 0 = nothing to try */
#define MFL_DISPLAY_FULLSCREEN 0      /* the video mode as videocfg.bin / the setup dialog chose it (the original) */
#define MFL_DISPLAY_WINDOWED 1        /* a framed window of the chosen size (clusters/cl_video_config_dialog.c) */
#define MFL_DISPLAY_BORDERLESS 2      /* a frameless window over the whole desktop, no mode switch */
int mfl_pref(int key);      /* value of one key (the file is read on the first call) */
extern int mfl_prefs[MFL_PREF_COUNT];      /* the values in force (the Preferences page changes the ones that apply at once) */
void mfl_settings_save(const int *values);      /* values[MFL_PREF_COUNT] -> every key written into the settings file (other lines kept) */
/* The Preferences page (recomp/mfl_settings_menu.c, page id MFL_PAGE_PREFS, a fourth row of the Options page); its texts are dat_get_string ids
   from MFL_TEXT_FIRST on (the game's tables end at 834) */
#define MFL_PAGE_PREFS 0x1b      /* the empty slot of g_menu_page_item_lists */
#define MFL_TEXT_FIRST 0x1000
void mfl_prefs_menu_update(void);      /* menu_update, input part: Left / Right change the selected row, Back writes the file */
void mfl_prefs_menu_draw(void);        /* hud_menu_draw_all_panels: the value beside each row and the description box */
void mfl_menu_update_scaled(void);     /* app_state_frontend_frame: menu_update with g_frame_seconds x menu_speed in the frontend state */
struct mfl_video_modes {      /* the Resolution row's modes, registered at start by the video code (clusters/cl_video_config_dialog.c, display_mode_apply):
                                 mode 0 = "Desktop" (resolution=auto), then the full-screen modes with 32 bit colour, >= 640 x 480, 4:3 or wider */
  int (*count)(void);                  /* modes offered (0 = no row) */
  const char *(*name)(int mode);       /* "1280x720" */
  int (*current)(void);                /* the mode of the key resolution= (or the one in use) */
  void (*choose)(int mode);            /* the page's choice: written as resolution= by mfl_settings_save, applies at the next start */
};
extern const struct mfl_video_modes *mfl_video_modes_api;
const unsigned short *mfl_prefs_text(int id);      /* dat_get_string for ids >= MFL_TEXT_FIRST: length-prefixed UTF-16 */
#endif

#ifndef MFL_FIX_LANG      /* a start without -vs / -cs / -l options (the exe run directly, not through launch.exe) acts as launch.exe's
                             `-VS0 -CS0 -L<n>`: no video / controller setup dialog (the game's own -VS0 -CS0 path: saved videocfg.bin /
                             contcfg%d.bin, else its defaults) and n = the Windows UI language (GetUserDefaultUILanguage) when its speech folder
                             toastaudio\pc\audio\pcdics\<name> is installed, else English.  Game language ids: 0 English, 1 French, 2 German,
                             3 Spanish, 4 Italian, 5 USA.  An option given on the command line wins; with MFL_FIX_PREFS the keys language and
                             setup_dialogs override (recomp/mfl_resources.c, called from app_initialise_on_bootup in automatch_near/an_hk_916.c) */
#define MFL_FIX_LANG 0
#endif
#if MFL_FIX_LANG
void mfl_lang_apply(void);
int mfl_lang_available(int language);      /* 1 when the game can play that language id: -1 (auto), 0 and 5 always, 1..4 when its speech folder exists */
#endif

#ifndef MFL_FIX_ALT_TAB      /* the window counts as active again when it is activated by a click or while minimised (WM_ACTIVATE low word), so after
                                Alt+Tab / the Windows key back into the game it runs and reads keys again; the original compared the whole wparam with
                                WA_ACTIVE and stayed paused (clusters/cl_window_create_cb_4a9830.c; agents/gfx_w17/notes.md) */
#define MFL_FIX_ALT_TAB 0
#endif

#ifndef MFL_FIX_WINDOW_ICON      /* the "DtS" window class gets the exe's own icon (icon group resource id 1, LoadIconA(hinstance, 1)) instead of none, so the title bar,
                                    Alt+Tab and the taskbar show it; an exe without that resource (the pinned VC7 link has none) gets NULL from LoadIconA = the
                                    original no-icon behaviour (tier1/window_create.c) */
#define MFL_FIX_WINDOW_ICON 0
#endif

#ifndef MFL_FIX_LOAD_SAVE_EARLY      /* the game save is read and applied at the end of app_boot_init (the boot loading screen, its spinning CD, still shows), and the
                                   load paths (title Load: state machine 2, return to the main menu: state machine 1) show no confirm box ("0x21a"), no
                                   "Loading" box with its 1 s wait and no "Load successful" box; the save side (write, autosave) is unchanged and the
                                   MFL_FIX_PREFS key skip_save_wait stays as it is (clusters/cl_app_boot_init.c, cl_gamesave_load_state_machine.c,
                                   automatch_near/an_hk_907.c, an_hk_9109.c; output/save_early_study_2026-11-21.md) */
#define MFL_FIX_LOAD_SAVE_EARLY 0
#endif
#if MFL_FIX_LOAD_SAVE_EARLY
extern int mfl_save_loaded_at_boot;      /* an_hk_9109.c: 1 after the boot-time load found and applied the save file */
void mfl_load_save_at_boot(void);      /* an_hk_9109.c: gamesave_exists, gamesave_load, gamesave_apply once; no message boxes */
#endif

#ifndef MFL_FIX_CRASHLOG    /* v143 builds only: an abnormal end (unhandled / fatal exception, CRT abort / invalid parameter / pure call, exit or ExitProcess
                                 with a code other than 0) appends a report to mflng-crash.txt beside the game's files (recomp/mfl_crashlog.c, installed
                                 from .CRT$XIB; build_overlay.py leaves the file out of every build without this switch) */
#define MFL_FIX_CRASHLOG 0
#endif

#ifndef MFL_FIX_FPS_CAP      /* the main loop waits out its 60 Hz frame budget against a fixed schedule in 1 ms Sleeps (timer period 1 ms), not in one
                                Sleep of whole ms per frame, so Sleep granularity no longer slows the game (Wine: ~50 fps = 84 % game speed, the simulation
                                is one 1/60 s tick per frame); the rate stays 60 (clusters/cl_app_main_loop.c; agents/fps_w17/notes.md) */
#define MFL_FIX_FPS_CAP 0
#endif

#ifndef MFL_FIX_FIXED_STEP      /* game speed independent of the frame rate: each frame runs as many 1/60 s simulation steps as real time has passed
                                   (round to nearest, the rest carried; at most 8 per frame; a frame over 250 ms or the first frame after a race-phase or app-state
                                   change = 1 step), through the game's own step
                                   loop; button presses are latched per step, the voice-queue timers / sound pump and the game-time counter run per
                                   step; at a steady 60 fps every frame is one step as in the original.  With MFL_FIX_FPS_CAP + MFL_FIX_PREFS the key
                                   fps_cap sets the limiter (automatch_near/an_hk_621.c, an_hk_789.c, an_hk_9223.c, tier1/debug_keys_and_profile.c,
                                   clusters/cl_app_main_loop.c; agents/fps_w17/notes.md) */
#define MFL_FIX_FIXED_STEP 0      /* opt-in, not in MFLng.exe (the comment keeps it out of build_mflng.sh / link_ci.sh FIXES=all) */
#endif
#if MFL_FIX_FIXED_STEP
void mfl_race_steps_run(void);      /* an_hk_789.c: this frame's race_start_setup(50) steps with the input record of each step (also the tutorial loop) */
#endif

/* The files of the sound functions with random-draw sites (agents/rep_w12/audio_rng_sites.txt) define MFL_AUDIO_RNG_SITES before including this
   header as their first lines; with the switch on, their wrapper calls (both spellings of the names) go to the audio-stream copies. */
#if MFL_FIX_DETERMINISTIC_AUDIO_RNG
#ifdef MFL_AUDIO_RNG_SITES
#define random_range mfl_audio_random_range
#define FUN_00481fd0 mfl_audio_random_range
#define rp_random_int_range mfl_audio_rp_random_int_range
#define FUN_004c45c0 mfl_audio_rp_random_int_range
#endif
#endif

#endif
