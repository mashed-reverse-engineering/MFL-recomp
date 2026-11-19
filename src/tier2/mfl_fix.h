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
                              default is the original behaviour, so without the file the game plays as the original (recomp/mfl_resources.c reader;
                              keys and sites: public/docs/MFL_FIX_FLAGS.md) */
#define MFL_FIX_PREFS 0
#endif
#if MFL_FIX_PREFS
enum mfl_pref_key {
  MFL_PREF_SKIP_INTRO,            /* skip_intro=1: the intro movies are not played */
  MFL_PREF_SKIP_SAVE_WAIT,        /* skip_save_wait=1: save / load message boxes close when the write is done, not after 1 s */
  MFL_PREF_UNLOCK_ALL,            /* unlock_all=1: MFL_FIX_UNLOCK_ALL at run time */
  MFL_PREF_WINNER_CAM_ROTATION,   /* winner_cam_rotation=0: the round-end camera zooms to the winning car without circling it */
  MFL_PREF_COUNT
};
int mfl_pref(int key);      /* value of one key (the file is read on the first call) */
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
