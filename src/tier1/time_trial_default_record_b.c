#include "mfl_tier2.h"
/*007eebc0*/ extern struct GameState g_game_state;
/* time_trial_default_record_b */

/* "(track, out fraction, out seconds, out minutes): second default time table, same output layout,
   starts from 9:59.99" */

void time_trial_default_record_b(uint param_1,undefined4 *param_2,undefined4 *param_3,undefined4 *param_4)
{
  *param_2 = 0x3f7d70a4;
  *param_3 = 0x3b;
  *param_4 = 9;
  switch (param_1) {
  case 30:
    if (g_game_state.mirror_mode == 0) {
      *param_2 = 0x3f4a3d80;
      *param_3 = 0x14;
      *param_4 = 0;
      return;
    }
    *param_2 = 0x3e947b00;
    *param_3 = 0x12;
    *param_4 = 0;
    return;
  case 1:
    *param_2 = 0x3dcccd00;
    *param_3 = 0xc;
    *param_4 = 0;
    return;
  case 3:
    if (g_game_state.mirror_mode != 0) {
      *param_2 = 0x3f400000;
      *param_3 = 0x11;
      *param_4 = 0;
      return;
    }
    *param_2 = 0x3f4a3d80;
    *param_3 = 0x14;
    *param_4 = 0;
    return;
  case 4:
    *param_2 = 0x3e947b00;
    *param_3 = 0x12;
    *param_4 = 0;
    return;
  case 39:
    if (g_game_state.mirror_mode == 0) {
      *param_2 = 0x3f47ae20;
      *param_3 = 0x1a;
      *param_4 = 0;
      return;
    }
    *param_2 = 0x3e051e80;
    *param_3 = 0x1b;
    *param_4 = 0;
    return;
  case 5:
    *param_2 = 0x3f147ae0;
    *param_3 = 0x18;
    *param_4 = 0;
    return;
  case 17:
    if (g_game_state.mirror_mode == 0) {
      *param_2 = 0x3f68f5c0;
      *param_3 = 0x11;
      *param_4 = 0;
      return;
    }
    *param_2 = 0x3f5c2900;
    *param_3 = 0x11;
    *param_4 = 0;
    return;
  case 20:
    *param_2 = 0x3e800000;
    *param_3 = 0x14;
    *param_4 = 0;
    return;
  case 38:
    if (g_game_state.mirror_mode == 0) {
      *param_2 = 0x3eae1480;
      *param_3 = 0x28;
      *param_4 = 0;
      return;
    }
    *param_2 = 0x3e6b8500;
    *param_3 = 0x20;
    *param_4 = 0;
    return;
  case 12:
    *param_2 = 0x3f07ae00;
    *param_3 = 0x21;
    *param_4 = 0;
    return;
  case 24:
    if (g_game_state.mirror_mode == 0) {
      *param_2 = 0x3e428f80;
      *param_3 = 0x12;
      *param_4 = 0;
      return;
    }
    *param_2 = 0x3cf5c400;
    *param_3 = 0x11;
    *param_4 = 0;
    return;
  case 18:
    *param_2 = 0x3e75c280;
    *param_3 = 0x11;
    *param_4 = 0;
    return;
  case 11:
    if (g_game_state.mirror_mode == 0) {
      *param_2 = 0x3f6147c0;
      *param_3 = 0x21;
      *param_4 = 0;
      return;
    }
    *param_2 = 0x3f47ae00;
    *param_3 = 0x21;
    *param_4 = 0;
    return;
  case 13:
    *param_2 = 0x3f75c280;
    *param_3 = 0x1a;
    *param_4 = 0;
    return;
  case 25:
    if (g_game_state.mirror_mode == 0) {
      *param_2 = 0x3d23d800;
      *param_3 = 0x1d;
      *param_4 = 0;
      return;
    }
    *param_2 = 0x3ed1eb80;
    *param_3 = 0x17;
    *param_4 = 0;
    return;
  case 8:
    *param_2 = 0x3f147ae0;
    *param_3 = 0x17;
    *param_4 = 0;
    return;
  case 2:
    if (g_game_state.mirror_mode == 0) {
      *param_2 = 0x3f3ae140;
      *param_3 = 0x1b;
      *param_4 = 0;
      return;
    }
    *param_2 = 0x3e570a00;
    *param_3 = 0x18;
    *param_4 = 0;
    return;
  case 6:
    *param_2 = 0x3ec28f40;
    *param_3 = 0x1b;
    *param_4 = 0;
    return;
  case 36:
    if (g_game_state.mirror_mode == 0) {
      *param_2 = 0x3f1999a0;
      *param_3 = 0xf;
      *param_4 = 0;
      return;
    }
    *param_2 = 0x3f6e1480;
    *param_3 = 0xf;
    *param_4 = 0;
    return;
  case 15:
    *param_2 = 0x3f75c290;
    *param_3 = 0xe;
    *param_4 = 0;
    return;
  case 34:
    if (g_game_state.mirror_mode == 0) {
      *param_2 = 0x3ed1eb80;
      *param_3 = 0x1d;
      *param_4 = 0;
      return;
    }
    *param_2 = 0;
    *param_3 = 0x16;
    *param_4 = 0;
    return;
  case 9:
    *param_2 = 0x3e6b8500;
    *param_3 = 0x17;
    *param_4 = 0;
    return;
  case 0:
    if (g_game_state.mirror_mode == 0) {
      *param_2 = 0x3e851ec0;
      *param_3 = 0x14;
      *param_4 = 0;
      return;
    }
    *param_2 = 0x3f23d700;
    *param_3 = 0x11;
    *param_4 = 0;
    return;
  case 37:
    if (g_game_state.mirror_mode == 0) {
      *param_2 = 0x3f428f60;
      *param_3 = 0x1f;
      *param_4 = 0;
      return;
    }
    *param_2 = 0x3f23d700;
    *param_3 = 0x1e;
    *param_4 = 0;
    return;
  case 33:
    if (g_game_state.mirror_mode == 0) {
      *param_2 = 0x3ec28f80;
      *param_3 = 0x30;
      *param_4 = 0;
      return;
    }
    *param_2 = 0x3ef5c280;
    *param_3 = 0x2f;
    *param_4 = 0;
    return;
  case 26:
    if (g_game_state.mirror_mode == 0) {
      *param_2 = 0x3f547ae0;
      *param_3 = 0x18;
      *param_4 = 0;
      return;
    }
    *param_2 = 0x3f000000;
    *param_3 = 0x19;
    *param_4 = 0;
    return;
  case 7:
  case 10:
  case 14:
  case 16:
  case 19:
  case 21:
  case 22:
  case 23:
  case 27:
  case 28:
  case 29:
  case 31:
  case 32:
  case 35:
  default:
    *param_2 = 0x3f7d70a4;
    *param_3 = 0x3b;
    *param_4 = 0;
    return;
  }
  return;
}
