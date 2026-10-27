#include "mfl_tier2.h"
/*007eebc0*/ extern struct GameState g_game_state;
/* time_trial_default_record_b */

/* "(track, out fraction, out seconds, out minutes): second default time table, same output layout,
   starts from 9:59.99" */

void time_trial_default_record_b(uint track,undefined4 *out_fraction,undefined4 *out_seconds,undefined4 *out_minutes)
{
  *out_fraction = 0x3f7d70a4;
  *out_seconds = 0x3b;
  *out_minutes = 9;
  switch (track) {
  case 30:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3f4a3d80;
      *out_seconds = 0x14;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3e947b00;
    *out_seconds = 0x12;
    *out_minutes = 0;
    return;
  case 1:
    *out_fraction = 0x3dcccd00;
    *out_seconds = 0xc;
    *out_minutes = 0;
    return;
  case 3:
    if (g_game_state.mirror_mode != 0) {
      *out_fraction = 0x3f400000;
      *out_seconds = 0x11;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3f4a3d80;
    *out_seconds = 0x14;
    *out_minutes = 0;
    return;
  case 4:
    *out_fraction = 0x3e947b00;
    *out_seconds = 0x12;
    *out_minutes = 0;
    return;
  case 39:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3f47ae20;
      *out_seconds = 0x1a;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3e051e80;
    *out_seconds = 0x1b;
    *out_minutes = 0;
    return;
  case 5:
    *out_fraction = 0x3f147ae0;
    *out_seconds = 0x18;
    *out_minutes = 0;
    return;
  case 17:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3f68f5c0;
      *out_seconds = 0x11;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3f5c2900;
    *out_seconds = 0x11;
    *out_minutes = 0;
    return;
  case 20:
    *out_fraction = 0x3e800000;
    *out_seconds = 0x14;
    *out_minutes = 0;
    return;
  case 38:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3eae1480;
      *out_seconds = 0x28;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3e6b8500;
    *out_seconds = 0x20;
    *out_minutes = 0;
    return;
  case 12:
    *out_fraction = 0x3f07ae00;
    *out_seconds = 0x21;
    *out_minutes = 0;
    return;
  case 24:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3e428f80;
      *out_seconds = 0x12;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3cf5c400;
    *out_seconds = 0x11;
    *out_minutes = 0;
    return;
  case 18:
    *out_fraction = 0x3e75c280;
    *out_seconds = 0x11;
    *out_minutes = 0;
    return;
  case 11:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3f6147c0;
      *out_seconds = 0x21;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3f47ae00;
    *out_seconds = 0x21;
    *out_minutes = 0;
    return;
  case 13:
    *out_fraction = 0x3f75c280;
    *out_seconds = 0x1a;
    *out_minutes = 0;
    return;
  case 25:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3d23d800;
      *out_seconds = 0x1d;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3ed1eb80;
    *out_seconds = 0x17;
    *out_minutes = 0;
    return;
  case 8:
    *out_fraction = 0x3f147ae0;
    *out_seconds = 0x17;
    *out_minutes = 0;
    return;
  case 2:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3f3ae140;
      *out_seconds = 0x1b;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3e570a00;
    *out_seconds = 0x18;
    *out_minutes = 0;
    return;
  case 6:
    *out_fraction = 0x3ec28f40;
    *out_seconds = 0x1b;
    *out_minutes = 0;
    return;
  case 36:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3f1999a0;
      *out_seconds = 0xf;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3f6e1480;
    *out_seconds = 0xf;
    *out_minutes = 0;
    return;
  case 15:
    *out_fraction = 0x3f75c290;
    *out_seconds = 0xe;
    *out_minutes = 0;
    return;
  case 34:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3ed1eb80;
      *out_seconds = 0x1d;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0;
    *out_seconds = 0x16;
    *out_minutes = 0;
    return;
  case 9:
    *out_fraction = 0x3e6b8500;
    *out_seconds = 0x17;
    *out_minutes = 0;
    return;
  case 0:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3e851ec0;
      *out_seconds = 0x14;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3f23d700;
    *out_seconds = 0x11;
    *out_minutes = 0;
    return;
  case 37:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3f428f60;
      *out_seconds = 0x1f;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3f23d700;
    *out_seconds = 0x1e;
    *out_minutes = 0;
    return;
  case 33:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3ec28f80;
      *out_seconds = 0x30;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3ef5c280;
    *out_seconds = 0x2f;
    *out_minutes = 0;
    return;
  case 26:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3f547ae0;
      *out_seconds = 0x18;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3f000000;
    *out_seconds = 0x19;
    *out_minutes = 0;
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
    *out_fraction = 0x3f7d70a4;
    *out_seconds = 0x3b;
    *out_minutes = 0;
    return;
  }
  return;
}
