// cluster-entries: race_check_winner_by_game_mode
/* Installed cluster race_check_winner_by_game_mode (tools/cluster_install.py from output/cluster_pilot_kb/race_check_winner_by_game_mode.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
typedef int CourseId;
/*00406cf0*/ extern int fx_objects_all_exploded();
/*0040cc60*/ extern int player_get_points();
/*0040fdb0*/ extern int get_player_count();
/*0040fdd0*/ extern int race_set_phase();
/*0041a380*/ extern int ai_ftol_noop_wrapper();
/*0042b110*/ extern enum CourseId current_course_id();
/*00434a10*/ extern int is_team_mode();
/*00434bb0*/ extern int get_event_type();
/*00451c50*/ extern int camera_records_warmup();
/*00459090*/ extern undefined __cdecl course_set_lod(undefined4 p0);
/*00464db0*/ extern int powerups_set_all_enabled();
/*00467d90*/ extern int weapons_unequip_all_a();
/*00467df0*/ extern int weapons_unequip_all_b();
/*0047a830*/ extern int car_is_active();
/*0047ac30*/ extern int car_get_mode();
/*005dd32c*/ extern float k_f_3;
/*005ddb20*/ extern float k_f_0_9;
/*005e64a4*/ extern float k_f_0;
/*005ddf58*/ extern float k_f_1_999;
/*00652df0*/ extern undefined4 g_intro_652df0;
/*00652df4*/ extern float g_intro_652df4;
/*00652df8*/ extern undefined4 g_car_652df8;
/*00652dfc*/ extern float g_intro_652dfc;
/*00652e00*/ extern float g_game_652e00;
/*007ef7b0*/ extern undefined4 g_car_7ef7b0;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7cc*/ extern float g_course_7ef7cc;
/*008d8b14*/ extern undefined4 g_race_setup;
/*008d8b30*/ extern undefined4 player_count;
/*0041a370*/ extern float ai_get_counter();
/* race_check_winner_by_game_mode */
#define RACE_SETUP_CAR_ACTIVE_FIRST (0xb8) /* g_race_setup +0xb8..+0xc7: one active/enabled word per car, 4 cars (inferred) */
#define RACE_SETUP_CAR_ACTIVE_END (200)
#define COURSE_HIGHWAYN (12)
#define COURSE_HIGHWAY (38)

/* "computes the winning player (points 8/12 thresholds, team mode, per game mode 4,5,7,8,9) into
   g_intro_652df0" */

int race_check_winner_by_game_mode(void)

{
  bool has_next_player;
  int scratch;
  int points;
  int scratch2;
  int team_mode;
  CourseId course_id;
  int winner_plus1;
  float10 counter;
  struct {
    int mode_out0;
    undefined1 mode_out1 [4];
    float counter1_f;
  } frame;
  
  winner_plus1 = 0;
  g_intro_652df0 = 0;
  scratch = get_event_type();
  if ((((scratch == EVENT_TIME_TRIAL_0) || (scratch = get_event_type(), scratch == EVENT_TIME_TRIAL_1)) ||
      (scratch = get_event_type(), scratch == EVENT_TIME_TRIAL_2)) || (scratch = get_event_type(), scratch == EVENT_TRAINING)) {
    return 0;
  }
  scratch = 1;
  do {
    points = player_get_points(scratch + -1);
    if ((((player_count == 2) || (player_count == 3)) || (scratch2 = is_team_mode(), scratch2 != 0)) &&
       (points == 8)) {
      winner_plus1 = scratch;
    }
    if (((game_mode == GAME_MODE_HARE_AND_HOUNDS) || (game_mode == GAME_MODE_HOLD_THE_FLAG)) && (points == 8)) {
      winner_plus1 = scratch;
    }
    if ((player_count == 4) && (0xb < points)) {
      winner_plus1 = scratch;
    }
    has_next_player = scratch < 4;
    scratch = scratch + 1;
  } while (has_next_player);
  if (winner_plus1 != 0) {
    points = 0;
    scratch = RACE_SETUP_CAR_ACTIVE_FIRST;
    do {
      if ((*(int *)(g_race_setup + scratch) != 0) && (scratch2 = car_is_active(points), scratch2 == 1)) {
        scratch2 = player_get_points(points);
        if (((player_count == 2) ||
            ((player_count == 3 || (team_mode = is_team_mode(), team_mode != 0)))) && (scratch2 == 8))
        {
          winner_plus1 = points + 1;
        }
        if ((player_count == 4) && (0xb < scratch2)) {
          winner_plus1 = points + 1;
        }
      }
      scratch = scratch + 4;
      points = points + 1;
    } while (scratch < RACE_SETUP_CAR_ACTIVE_END);
  }
  switch(game_mode) {
  case GAME_MODE_RACE:
    winner_plus1 = ai_ftol_noop_wrapper(0);
    winner_plus1 = winner_plus1 + 1;
    break;
  case GAME_MODE_KILL_COPTER:
    winner_plus1 = car_is_active(0);
    if (winner_plus1 == 0) {
      winner_plus1 = -1;
      goto LAB_0041280c;
    }
    winner_plus1 = fx_objects_all_exploded();
    break;
  case GAME_MODE_AIRSTRIKE:
    points = 0;
    scratch = get_player_count();
    if (0 < scratch) {
      do {
        counter = (float10)ai_get_counter(points);
        if (counter > (float10)k_f_3) {
          winner_plus1 = points + 1;
          break;
        }
        points = points + 1;
        scratch = get_player_count();
      } while (points < scratch);
    }
    break;
  case GAME_MODE_SURVIVAL:
    car_get_mode(0,&frame.mode_out0,frame.mode_out1);
    winner_plus1 = (uint)(frame.mode_out0 == 0) * 2 + -1;
    goto LAB_0041280c;
  case GAME_MODE_CHASE_FUGITIVE:
    winner_plus1 = ai_ftol_noop_wrapper(0);
    if (winner_plus1 != -1) {
LAB_004129e1:
      winner_plus1 = -1;
      g_car_7ef7b0 = 0;
      goto LAB_0041280c;
    }
    car_get_mode(0,&frame.mode_out0,frame.mode_out1);
    if (frame.mode_out0 != 0) {
      winner_plus1 = -1;
      g_car_7ef7b0 = 0;
      goto LAB_0041280c;
    }
    counter = (float10)ai_get_counter(1);
    frame.counter1_f = (float)counter;
    counter = (float10)ai_get_counter(0);
    if ((float10)k_f_0_9 <= (float10)frame.counter1_f - counter) {
      winner_plus1 = -1;
      g_car_7ef7b0 = 0;
      goto LAB_0041280c;
    }
    goto LAB_004129c3;
  case GAME_MODE_BEAT_THE_BOMB:
    car_get_mode(0,&frame.mode_out0,frame.mode_out1);
    if (frame.mode_out0 != 0) {
      winner_plus1 = -1;
      g_car_7ef7b0 = 0;
      goto LAB_0041280c;
    }
    counter = (float10)ai_get_counter(0);
    if (!(counter >= (float10)k_f_1_999)) {      /* NaN enters (original: test ah,1; jne) */
      if (k_f_0 <= g_course_7ef7cc) {      /* original: fld 0.0; fcomp g; test ah,0x41; jp */
        return 0;
      }
      goto LAB_004129e1;
    }
LAB_004129c3:
    winner_plus1 = 1;
    g_car_7ef7b0 = 1;
    goto LAB_0041280c;
  }
  if (winner_plus1 == 0) {
    return 0;
  }
LAB_0041280c:
  weapons_unequip_all_a();
  weapons_unequip_all_b();
  powerups_set_all_enabled(0);
  race_set_phase(RACE_PHASE_ROUND_END);
  g_intro_652df0 = 1;
  g_intro_652df4 = 700.0f;
  g_intro_652dfc = 0;
  g_game_652e00 = 0;
  if (winner_plus1 == -1) {
    camera_records_warmup(1,0);
    g_car_652df8 = 1;
    g_car_7ef7b0 = 0;
  }
  else {
    scratch = winner_plus1 + -1;
    camera_records_warmup(1,scratch);
    g_car_7ef7b0 = (uint)(scratch == 0);
    g_car_652df8 = scratch;
  }
  course_id = current_course_id();
  if ((course_id == COURSE_HIGHWAY) || (course_id = current_course_id(), course_id == COURSE_HIGHWAYN)) {
    course_set_lod(0);
  }
  return winner_plus1;
}

