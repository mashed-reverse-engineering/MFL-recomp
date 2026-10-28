#include "mfl_tier2.h"
/*00406cf0*/ extern int fx_objects_all_exploded();
/*00409eb0*/ extern double player_rank_key();
/*00409ed0*/ extern int player_set_precise_position();
/*00409f30*/ extern double player_progress_b();
/*0040fbf0*/ extern int race_get_focus_player();
/*0040fdb0*/ extern int get_player_count();
/*00410f10*/ extern int race_battle_mode_update();
/*00426b40*/ extern int car_kill();
/*00427690*/ extern int __cdecl player_get_last_hit_by(undefined4 p0);
/*00434bb0*/ extern int get_event_type();
/*00437f10*/ extern int training_lesson_get();
/*00437f20*/ extern int ui_get_car_8c86c8();
/*00437f40*/ extern int ui_get_car_8c8070();
/*0044c2b0*/ extern double camera_get_8c7e00();
/*0044c540*/ extern int camera_get_car_8c747c();
/*0047a830*/ extern int car_is_active();
/*0047ac30*/ extern int car_get_mode();
/*004a1680*/ extern int player_rumble();
/*005dd32c*/ extern float k_f_3;
/*005dd620*/ extern float float_10_0;
/*005dd62c*/ extern float k_f_100;
/*005dd7f4*/ extern float k_f_80;
/*005ddc28*/ extern float k_f_0;
/*005ddea0*/ extern float k_f_20;
/*005ddf58*/ extern float k_f_1_999;
/*005ddf74*/ extern float k_f_64;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7bc*/ extern undefined4 g_cars_7ef7bc;
/*007ef7cc*/ extern float g_course_7ef7cc;
/*00801b08*/ extern undefined4 g_game_801b08;
/*008d8b14*/ extern char *g_race_setup;
/*0041a370*/ extern float ai_get_counter();
#define RACE_SLOT_TABLE_START (0xb8)
#define RACE_SLOT_TABLE_END (200)
/* game_mode_end_check */

/* switch on game_mode; contains the compare against [008c7e00]==10.0 patched by the trainer 'max
   distance' (008c7e00 has no static writer) */

/* Checks whether the current race/battle has ended (inferred).
   Takes no parameters. Returns 1 when the end condition holds, 0 to keep running.
   Side effect: g_game_801b08 is set to the number of active race slots.
   Locals are reused across roles, see the block comments below. */
uint game_mode_end_check(void)

{
  bool counter_below_limit;
  float focus_player;
  int focus_car; /* car handle (an int, not a float) */
  int result;
  uint all_exploded;
  int car_active;
  int player_index;
  int trailing_car;
  int slot_offset;
  int car_index;
  double last_value;
  struct {
    float progress_a;
    float progress_b;
    int car0_mode;
    float max_rank_key;
    undefined1 car0_mode_extra [4];
    int player_b_mode;
    int focus_mode;
    int player_b;
    undefined1 focus_mode_extra [4];
    undefined1 player_b_mode_extra [4];
  } frame;

  result = camera_get_car_8c747c();
  if (result == 1) {
    return 0;
  }
  /* Per-mode end conditions. Modes not listed fall through to the common checks below. */
  switch(game_mode) {
  case 4:
    counter_below_limit = !(ai_get_counter(0) >= k_f_3);
    goto LAB_00413226;
  case 5:
    result = car_is_active(0);
    if (result == 0) {
      return 1;
    }
    all_exploded = fx_objects_all_exploded();
    return all_exploded;
  case 7:
    /* Loops over the players: slot_offset counts the inactive cars here (reused local).
       Returns 1 when at most one car is still active. */
    result = car_is_active(0);
    if (result == 0) {
      return 1;
    }
    player_index = 0;
    slot_offset = 0;
    result = get_player_count();
    if (0 < result) {
      do {
        result = car_is_active(player_index);
        if (result == 0) {
          slot_offset = slot_offset + 1;
        }
        if (ai_get_counter(player_index) > k_f_3) {
          return 1;
        }
        player_index = player_index + 1;
        result = get_player_count();
      } while (player_index < result);
    }
    result = get_player_count();
    return (uint)(result + -1 <= slot_offset);
  case 8:
    if (ai_get_counter(0) >= k_f_3) {
      return 1;
    }
    car_get_mode(0,&frame.car0_mode,frame.car0_mode_extra);
    if (frame.car0_mode != 0) {
      return 1;
    }
    break;
  case 9:
    if (ai_get_counter(0) >= k_f_3) {
      return 1;
    }
    if (ai_get_counter(1) >= k_f_3) {
      return 1;
    }
    car_get_mode(0,&frame.car0_mode,frame.car0_mode_extra);
    if (frame.car0_mode != 0) {
      return 1;
    }
    result = player_get_last_hit_by(1);
    if (result != 0) {
      return 1;
    }
    break;
  case 10:
    car_get_mode(0,&frame.car0_mode,frame.car0_mode_extra);
    if (frame.car0_mode != 0) {
      return 1;
    }
    if (g_course_7ef7cc <= k_f_0) {
      return 1;
    }
    counter_below_limit = !(ai_get_counter(0) >= k_f_1_999);
LAB_00413226:
    if (!counter_below_limit) {
      return 1;
    }
  }
  /* Training lesson end: event EVENT_TRAINING and either lesson 1 with player 0's
     rank key above k_f_64, or lesson 4 with ui ids 1 or 4 and then 3. */
  result = get_event_type();
  if ((result == EVENT_TRAINING) &&
     (((result = training_lesson_get(), result == 1 &&
       (float)player_rank_key(0) > k_f_64) ||
      ((result = training_lesson_get(), result == 4 &&
       (((result = ui_get_car_8c86c8(), result == 1 || (result = ui_get_car_8c86c8(), result == 4)) &&
        (result = ui_get_car_8c8070(), result == 3)))))))) {
    return 1;
  }
  /* Focus-camera case (camera value == float_10_0, inferred): compare the progress of the
     two focus players and kill the trailing one (inferred). progress_b first holds focus
     player A's handle (from race_get_focus_player), then player B's progress. */
  frame.max_rank_key = 0.0;
  last_value = (double)camera_get_8c7e00();
  if (last_value == (double)float_10_0) {
    race_get_focus_player(&frame.progress_b,&frame.player_b);
    focus_car = *(int *)&frame.progress_b;
    last_value = (double)player_progress_b(*(int *)&frame.progress_b);
    frame.progress_a = (float)last_value;
    last_value = (double)player_progress_b(frame.player_b);
    frame.progress_b = (float)last_value;
    if ((frame.progress_a > k_f_80) && (frame.progress_b < k_f_20)) {
      frame.progress_a = frame.progress_a - k_f_100;
    }
    if ((frame.progress_b > k_f_80) && (frame.progress_a < k_f_20)) {
      frame.progress_b = frame.progress_b - k_f_100;
    }
    if (game_mode == GAME_MODE_HOLD_THE_FLAG) {
      if (focus_car == g_cars_7ef7bc) {
        frame.progress_a = 100.0;
      }
      if (frame.player_b == g_cars_7ef7bc) {
        frame.progress_b = 100.0;
      }
    }
    car_get_mode(focus_car,&frame.focus_mode,frame.focus_mode_extra);
    car_get_mode(frame.player_b,&frame.player_b_mode,frame.player_b_mode_extra);
    trailing_car = focus_car;
    result = frame.focus_mode;
    if (frame.progress_a > frame.progress_b) {
      trailing_car = frame.player_b;
      result = frame.player_b_mode;
      frame.player_b = focus_car;
      frame.player_b_mode = frame.focus_mode;
    }
    if ((frame.player_b_mode != 0) && (trailing_car = frame.player_b, result != 0)) {
      return 0;
    }
    player_rumble(trailing_car,3,10,0x80);
    car_kill(trailing_car);
    result = race_battle_mode_update(trailing_car,1);
    if (result == 1) {
      return 1;
    }
  }
  /* Count occupied race slots (player_index, reused as the slot count here) and active
     cars among them (result, reused as the active count here, stored in g_game_801b08).
     Slots are at g_race_setup + slot_offset, 4 bytes apart. */
  player_index = 0;
  result = 0;
  car_index = 0;
  slot_offset = RACE_SLOT_TABLE_START;
  do {
    if (*(int *)(slot_offset + g_race_setup) != 0) {
      player_index = player_index + 1;
      car_active = car_is_active(car_index);
      if (car_active == 1) {
        result = result + 1;
      }
    }
    slot_offset = slot_offset + 4;
    car_index = car_index + 1;
  } while (slot_offset < RACE_SLOT_TABLE_END);
  g_game_801b08 = result;
  /* One occupied slot: end when no car is active. Otherwise end when exactly one car is
     active; when none are active, set the precise positions by rank key. */
  if (player_index == 1) {
    if (result == 0) {
      return 1;
    }
  }
  else {
    if (result == 1) {
      return 1;
    }
    if (result == 0) {
      /* Max rank key over the occupied slots (frame.max_rank_key). The second loop also
         copies it into focus_player (reused local) for the position calls below. */
      player_index = 0;
      result = RACE_SLOT_TABLE_START;
      g_game_801b08 = 0;
      do {
        if (*(int *)(result + g_race_setup) != 0) {
          last_value = (double)player_rank_key(player_index);
          frame.max_rank_key = (float)last_value;
        }
        result = result + 4;
        player_index = player_index + 1;
      } while (result < RACE_SLOT_TABLE_END);
      player_index = 0;
      result = RACE_SLOT_TABLE_START;
      do {
        if ((*(int *)(result + g_race_setup) != 0) &&
           (last_value = (double)player_rank_key(player_index), last_value > frame.max_rank_key)) {
          frame.max_rank_key = (float)last_value;
        }
        focus_player = frame.max_rank_key;
        result = result + 4;
        player_index = player_index + 1;
      } while (result < RACE_SLOT_TABLE_END);
      player_set_precise_position(0,*(int *)&frame.max_rank_key);
      player_set_precise_position(1,*(int *)&focus_player);
      player_set_precise_position(2,*(int *)&focus_player);
      player_set_precise_position(3,*(int *)&focus_player);
      return 1;
    }
  }
  return 0;
}


