#include "mfl_tier2.h"
extern char s_soundstuff_00617970[];
extern char s_more_soundstuff_006179f0[];
/*0042d970*/ extern int build_info_frame();
/*00469430*/ extern int sound_load_perm_dict();
/*0046ed30*/ extern int sound_language_path();
/*0046efd0*/ extern int sound_card_info_log();
/*006ad8dc*/ extern float g_sound_6ad8dc;
/*006ad8ec*/ extern undefined4 g_sound_6ad8ec;
/*006adcf0*/ extern undefined4 g_sound_6adcf0;
/*006ae0b0*/ extern float g_sound_6ae0b0;
/*006ae0b4*/ extern float g_sound_6ae0b4;
/*006ae0b8*/ extern float g_sound_6ae0b8;
/*006ae108*/ extern undefined4 g_sound_streams_enabled;
/*007ef744*/ extern undefined4 g_game_7ef744;
/*008d9960*/ extern undefined4 g_sound_8d9960;
/*008d9964*/ extern undefined4 g_sound_stuff_8d9964;
/*008d9968*/ extern undefined4 g_sound_stuff_8d9968;
/*008d996c*/ extern undefined4 g_sound_stuff_8d996c;
/* sound_stuff_init */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/*  */

/* Sound system startup: selects the language path, logs the sound card info, loads the
   permanent sound dictionary and resets the sound globals to their defaults. Takes no
   parameters and returns nothing. (inferred from callees and global names) */
void sound_stuff_init(void)

{
  /* Pick the language-specific sound path from the game language global. */
  sound_language_path(g_game_7ef744);
  /* Two info-frame messages from the two sound info strings (inferred). */
  build_info_frame(s_soundstuff_00617970,s_soundstuff_00617970,0);
  sound_card_info_log();
  build_info_frame(s_more_soundstuff_006179f0,s_more_soundstuff_006179f0,0);
  sound_load_perm_dict();
  /* Reset sound state globals to their defaults. */
  g_sound_8d9960 = 0;
  g_sound_stuff_8d9964 = 0;
  g_sound_stuff_8d9968 = 0;
  g_sound_6adcf0 = 0;
  g_sound_6ad8ec = 1000;
  g_sound_stuff_8d996c = 0;
  g_sound_streams_enabled = 1;
  g_sound_6ad8dc = 1.0f;
  g_sound_6ae0b0 = 0;
  g_sound_6ae0b4 = 0;
  g_sound_6ae0b8 = 0;
  return;
}


