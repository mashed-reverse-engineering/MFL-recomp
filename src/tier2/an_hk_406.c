#include "mfl_tier2.h"
/*00469b20*/ extern int sound_music_fade_step();
/*004a3490*/ extern int __cdecl app_get_app_76e890_3490(undefined4 p0);
/*004a3930*/ extern int sound_volume_to_directsound_db();
/*005dd330*/ extern float float_1_0;
/*005dd374*/ extern float k_f_0_2;
/*005ddc28*/ extern float k_f_0;
/*006ad8dc*/ extern float g_sound_6ad8dc;
/*006adcf4*/ extern float g_sound_frame_6adcf4;
/* sound_music_fade_step_b */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "second fade variable stepped by -0.2 / +0.2 like sound_music_fade_step" */

/*
 * Steps the music fade level g_sound_6ad8dc by one notch and pushes the result
 * to DirectSound, scaled by g_sound_frame_6adcf4.
 *   step_dir 0: fade down by k_f_0_2 (inferred)
 *   step_dir 1: fade up by k_f_0_2 (inferred)
 *   step_dir 2: force the level to 0.0 (inferred)
 *   other:      clamp the current level only (inferred)
 * The level is only sent to the sound layer when app_get_app_76e890_3490(0)
 * returns non-zero.
 */
void sound_music_fade_step_b(int step_dir)

{
  int app_status;

  /* step down (0); step up (1) jumps into the clamp below */
  if (step_dir == 0) {
    g_sound_6ad8dc = g_sound_6ad8dc - k_f_0_2;
LAB_00469be1:
    if (float_1_0 < g_sound_6ad8dc) {
      g_sound_6ad8dc = 1.0;
      goto LAB_00469c1d;
    }
    if (k_f_0 <= g_sound_6ad8dc) goto LAB_00469c1d;
  }
  else {
    if (step_dir == 1) {
      g_sound_6ad8dc = g_sound_6ad8dc + k_f_0_2;
      goto LAB_00469be1;
    }
    if (step_dir != 2) goto LAB_00469be1;
  }
  g_sound_6ad8dc = 0.0;
LAB_00469c1d:
  /* only push the level to the sound layer when the app status is non-zero */
  app_status = app_get_app_76e890_3490(0);
  if (app_status != 0) {
    sound_volume_to_directsound_db(g_sound_frame_6adcf4 * g_sound_6ad8dc);
  }
  return;
}


