#include "mfl_tier2.h"
/*007eebc0*/ extern struct GameState g_game_state;

/* CourseId values (names/MFL_enums.txt), the case labels of the switch below */
#define COURSE_ARCTIC (0)
#define COURSE_ANGELS (1)
#define COURSE_ROUABOUT (2)
#define COURSE_EGYPT (3)
#define COURSE_EGYPT_R (4)
#define COURSE_NEUST_R (5)
#define COURSE_TIERRA_N (6)
#define COURSE_SUPERG_N (8)
#define COURSE_FOREST_R (9)
#define COURSE_SANDS (11)
#define COURSE_HIGHWAYN (12)
#define COURSE_SANDSNOW (13)
#define COURSE_STORM_R (15)
#define COURSE_NT1 (17)
#define COURSE_DEMR (18)
#define COURSE_NT1_S (20)
#define COURSE_DEM (24)
#define COURSE_SUPERG (25)
#define COURSE_CITY (26)
#define COURSE_TRAINING (30)
#define COURSE_WARZONE (33)
#define COURSE_FOREST (34)
#define COURSE_STORM (36)
#define COURSE_DUMP (37)
#define COURSE_HIGHWAY (38)
#define COURSE_NEUSTEIN (39)
/* time_trial_default_record_a */

/* "(track, out fraction, out seconds, out minutes): default record time of a track (the first
   table, a different value when g_game_state.mirror_mode is set)" */
/* Purpose: writes the default record time of course_id (CourseId, inferred from the case labels)
   through the out parameters. out_fraction is a float fraction of a second (0..1, stored as its
   bit pattern: 0x3e051ec0 = 0.13f), out_seconds is whole seconds, out_minutes is always 0 here.
   With mirror_mode == 0 a row gives its first value; with mirror_mode set, the second one. */

/* TIER2: unvalidated 2026-10-18 authority=diffemu */
void time_trial_default_record_a(undefined4 course_id,undefined4 *out_fraction,undefined4 *out_seconds,undefined4 *out_minutes)
{
  /* One case per course. Cases that test mirror_mode give the second value when it is set;
     courses not listed here take the default row. */
  switch(course_id) {
  case COURSE_ARCTIC:
    break;
  case COURSE_ANGELS:
    *out_fraction = 0x3e051ec0;
    *out_seconds = 7;
    *out_minutes = 0;
    return;
  case COURSE_ROUABOUT:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3d75c300;
      *out_seconds = 0xd;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3e9eb860;
    *out_seconds = 10;
    *out_minutes = 0;
    return;
  case COURSE_EGYPT:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3ed1eb80;
      *out_seconds = 10;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3f428f60;
    *out_seconds = 8;
    *out_minutes = 0;
    return;
  case COURSE_EGYPT_R:
    *out_fraction = 0x3e570a40;
    *out_seconds = 8;
    *out_minutes = 0;
    return;
  case COURSE_NEUST_R:
    *out_fraction = 0x3f5c28f0;
    *out_seconds = 0xc;
    *out_minutes = 0;
    return;
  case COURSE_TIERRA_N:
    *out_fraction = 0x3eb851e0;
    *out_seconds = 0xd;
    *out_minutes = 0;
    return;
  default:
    *out_fraction = 0x3f7d70a4;
    *out_seconds = 0x3b;
    *out_minutes = 0;
    return;
  case COURSE_SUPERG_N:
    *out_fraction = 0x3ea8f5c0;
    *out_seconds = 0xb;
    *out_minutes = 0;
    return;
  case COURSE_FOREST_R:
    *out_fraction = 0x3f2e1480;
    *out_seconds = 0xc;
    *out_minutes = 0;
    return;
  case COURSE_SANDS:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3f547ae0;
      *out_seconds = 0xd;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3de14800;
    *out_seconds = 0x10;
    *out_minutes = 0;
    return;
  case COURSE_HIGHWAYN:
    *out_fraction = 0x3f000000;
    *out_seconds = 0x11;
    *out_minutes = 0;
    return;
  case COURSE_SANDSNOW:
    *out_fraction = 0x3f6e1480;
    *out_seconds = 0xc;
    *out_minutes = 0;
    return;
  case COURSE_STORM_R:
    *out_fraction = 0x3f4cccd0;
    *out_seconds = 7;
    *out_minutes = 0;
    return;
  case COURSE_NT1:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3d4ccd00;
      *out_seconds = 9;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3e9999a0;
    *out_seconds = 8;
    *out_minutes = 0;
    return;
  case COURSE_DEMR:
    *out_fraction = 0x3e23d700;
    *out_seconds = 0xb;
    *out_minutes = 0;
    return;
  case COURSE_NT1_S:
    *out_fraction = 0x3f6147b0;
    *out_seconds = 9;
    *out_minutes = 0;
    return;
  case COURSE_DEM:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0;
      *out_seconds = 0xc;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3e051ec0;
    *out_seconds = 10;
    *out_minutes = 0;
    return;
  case COURSE_SUPERG:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3f6e1480;
      *out_seconds = 0xc;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3f23d710;
    *out_seconds = 10;
    *out_minutes = 0;
    return;
  case COURSE_CITY:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3e9999a0;
      *out_seconds = 0xe;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3e570a40;
    *out_seconds = 0xf;
    *out_minutes = 0;
    return;
  case COURSE_TRAINING:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3f28f5c0;
      *out_seconds = 7;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3f30a3d8;
    *out_seconds = 6;
    *out_minutes = 0;
    return;
  case COURSE_WARZONE:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3dcccd00;
      *out_seconds = 0x13;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3f0a3d80;
    *out_seconds = 0x12;
    *out_minutes = 0;
    return;
  case COURSE_FOREST:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3cf5c200;
      *out_seconds = 0xe;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3c23d800;
    *out_seconds = 0xb;
    *out_minutes = 0;
    return;
  case COURSE_STORM:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3e3851e0;
      *out_seconds = 6;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3ee147a0;
    *out_seconds = 8;
    *out_minutes = 0;
    return;
  case COURSE_DUMP:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3f47ae10;
      *out_seconds = 0xf;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3ec28f60;
    *out_seconds = 0xf;
    *out_minutes = 0;
    return;
  case COURSE_HIGHWAY:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3f1c2900;
      *out_seconds = 0x15;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3ed1eb80;
    *out_seconds = 0x10;
    *out_minutes = 0;
    return;
  case COURSE_NEUSTEIN:
    if (g_game_state.mirror_mode == 0) {
      *out_fraction = 0x3e385200;
      *out_seconds = 0xd;
      *out_minutes = 0;
      return;
    }
    *out_fraction = 0x3f428f60;
    *out_seconds = 0xe;
    *out_minutes = 0;
    return;
  }
  /* COURSE_ARCTIC (case 0) breaks out of the switch and falls through to here. */
  if (g_game_state.mirror_mode == 0) {
    *out_fraction = 0x3f428f60;
    *out_seconds = 0xb;
    *out_minutes = 0;
    return;
  }
  *out_fraction = 0x3f400000;
  *out_seconds = 9;
  *out_minutes = 0;
  return;
}


