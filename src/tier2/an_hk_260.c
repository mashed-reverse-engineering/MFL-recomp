#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
/*005dd338*/ extern float k_f_0_01;
/*005dd374*/ extern float k_f_0_2;
/*005ddc28*/ extern float k_f_0;
/*006ad8ac*/ extern float g_sound_music_fade;
/* sound_music_fade_step */

/* "(dir): g_sound_6ad8ac -= 0.2 / += 0.2 / -= 0.01, clamped to 1.0" */

/* Steps the music fade level g_sound_music_fade by one fade operation, then clamps it.
 * fade_op: 0 = fade down by 0.2, 1 = fade up by 0.2, 2 = mute (set to 0.0),
 *          3 = fade down by 0.01 (inferred from the comment above).
 * Above 1.0 the level is set to 1.0; below k_f_0 it is set to 0.0. Returns nothing. */

void sound_music_fade_step(undefined4 fade_op)

{
  /* Apply the step selected by fade_op; case 2 jumps straight to the mute path. */
  switch(fade_op) {
  case 0:
    g_sound_music_fade = g_sound_music_fade - k_f_0_2;
    break;
  case 1:
    g_sound_music_fade = g_sound_music_fade + k_f_0_2;
    break;
  case 2:
    goto switchD_00469b29_caseD_2;
  case 3:
    g_sound_music_fade = g_sound_music_fade - k_f_0_01;
  }
  if (float_1_0 < g_sound_music_fade) {
    g_sound_music_fade = 1.0;
    return;
  }
  if (g_sound_music_fade < k_f_0) {
switchD_00469b29_caseD_2:
    g_sound_music_fade = 0.0;
  }
  return;
}


