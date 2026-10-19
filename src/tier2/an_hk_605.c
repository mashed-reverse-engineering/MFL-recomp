#include "mfl_tier2.h"
/*00409eb0*/ extern float player_rank_key();
/*00409f60*/ extern int grid_position();
/*0040a830*/ extern int course_lap_line_find_index();
/*0040c820*/ extern int player_add_points();
/*0040f710*/ extern int intro_banner_get_flag();
/*0040fdb0*/ extern int get_player_count();
/*0040fdc0*/ extern int race_get_phase();
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*00410f10*/ extern int race_battle_mode_update();
/*004247c0*/ extern int debris_slot_clear_forward();
/*00426b40*/ extern int car_kill();
/*0044c540*/ extern int camera_get_car_8c747c();
/*00467890*/ extern int player_status_set();
/*0047a100*/ extern int car_reset_state();
/*0047a620*/ extern undefined __cdecl car_reset_full(undefined4 p0);
/*0047a830*/ extern int car_is_active();
/*0047a850*/ extern int car_auto_steer_assist_step();
/*0047ac30*/ extern int car_get_mode();
/*0047ac60*/ extern int car_set_mode();
/*0047eb50*/ extern undefined __cdecl car_spawn(uint p0, RwReal p1, float p2, RwReal p3, RwReal p4, undefined4 p5);
/*00499a80*/ extern int fx_ring_clear();
/*007ef7b8*/ extern undefined4 game_mode;
/*00801b0c*/ extern undefined4 g_car_801b0c;

#define CAR_SLOTS (4)
#define RACE_PHASE_RESET_CARS (7)
/* car mode set when a timed mode runs out; the respawn/kill pass below acts on it (inferred) */
#define CAR_MODE_RESPAWN (0xff)
/* a mode 1 or 2 timer above this (i.e. 4000 ticks) moves the car to CAR_MODE_RESPAWN */
#define MODE_TIMEOUT_TICKS (3999)

/* cars_step_race_phase_and_respawn */

/* "per phase 7 car_reset_full; steps cars (race_update_common_sub_47a850); handles mode timers
   (4000 ticks), respawn via car_spawn/grid_position" */

/* Advances the race frame by step_ticks: resets cars on race phase 7, steps every active car,
   ticks each car's mode timer, and respawns or kills cars whose timer ran out.
   Returns 1. (inferred: the only return value) */
undefined4 cars_step_race_phase_and_respawn(int step_ticks)

{
  int race_phase;
  int active_count;
  int respawn_count;
  int timer;
  uint banner_car;
  uint car_index;
  struct {
    undefined4 local_38;
    RwReal local_34;
    int local_30;
    RwReal local_2c;
    float local_28;
    RwReal local_24;
    int local_20 [4];
    int local_10 [4];
  } frame;
  
  get_player_count();
  race_phase = race_get_phase();
  /* race phase 7: reset every active car (inferred meaning) */
  if (race_phase == RACE_PHASE_RESET_CARS) {
    race_phase = 0;
    do {
      active_count = race_setup_slot_used(race_phase);
      if (active_count != 0) {
        active_count = car_is_active(race_phase);
        if (active_count == 1) {
          car_reset_full(race_phase);
        }
      }
      race_phase = race_phase + 1;
    } while (race_phase < CAR_SLOTS);
  }
  /* pass 1: count the active cars (active_count) and fetch each car's mode and timer */
  frame.local_30 = 0;
  race_phase = 0;
  active_count = 0;
  do {
    respawn_count = race_setup_slot_used(race_phase);
    if (respawn_count != 0) {
      respawn_count = car_is_active(race_phase);
      if (respawn_count == 1) {
        active_count = active_count + 1;
        car_auto_steer_assist_step(race_phase,step_ticks);
        car_get_mode(race_phase,frame.local_20 + race_phase,frame.local_10 + race_phase);
      }
    }
    race_phase = race_phase + 1;
  } while (race_phase < CAR_SLOTS);
  /* pass 2: tick each active car's mode timer (timer). race_phase counts cars in a mode;
     respawn_count is +1 per car sent to CAR_MODE_RESPAWN and -1 in the game_mode 8/9/7/4 branches */
  race_phase = 0;
  respawn_count = 0;
  car_index = 0;
  frame.local_30 = active_count;
  do {
    active_count = race_setup_slot_used(car_index);
    if (active_count != 0) {
      active_count = car_is_active(car_index);
      if ((active_count == 1) && (active_count = frame.local_20[car_index], active_count != 0)) {
        race_phase = race_phase + 1;
        timer = frame.local_10[car_index] + step_ticks;
        frame.local_10[car_index] = timer;
        if (((active_count == 1) || (active_count == 2)) && (MODE_TIMEOUT_TICKS < timer)) {
          frame.local_20[car_index] = CAR_MODE_RESPAWN;
        }
        car_set_mode(car_index,frame.local_20[car_index],timer);
        if (frame.local_20[car_index] == CAR_MODE_RESPAWN) {
          active_count = respawn_count + 1;
          timer = camera_get_car_8c747c();
          if ((timer != 0) && (game_mode != 10)) {
            banner_car = intro_banner_get_flag();
            if (banner_car == car_index) {
              frame.local_10[car_index] = 0;
              frame.local_20[car_index] = 0;
              car_set_mode(car_index,0,0);
              debris_slot_clear_forward(car_index);
              fx_ring_clear();
              car_reset_state(car_index);
              frame.local_38 = (int)player_rank_key(car_index);
              course_lap_line_find_index(&frame.local_38);
              grid_position(frame.local_38,0,1,&frame.local_2c,&frame.local_34);
              car_spawn(car_index,frame.local_2c,frame.local_28,frame.local_24,frame.local_34,&g_car_801b0c);
              active_count = respawn_count;
            }
          }
          respawn_count = active_count;
          if (game_mode == 8) {
            if (car_index == 0) goto LAB_00428da3;
            frame.local_10[car_index] = 0;
            frame.local_20[car_index] = 0;
            car_set_mode(car_index,0,0);
            debris_slot_clear_forward(car_index);
            fx_ring_clear();
            car_reset_state(car_index);
            frame.local_38 = (int)player_rank_key(car_index);
            course_lap_line_find_index(&frame.local_38);
            grid_position(frame.local_38,car_index,4,&frame.local_2c,&frame.local_34);
            car_spawn(car_index,frame.local_2c,frame.local_28,frame.local_24,frame.local_34,&g_car_801b0c);
            respawn_count = respawn_count + -1;
          }
          if ((game_mode == 9) || (game_mode == 7)) {
            frame.local_10[car_index] = 0;
            frame.local_20[car_index] = 0;
            car_set_mode(car_index,0,0);
            debris_slot_clear_forward(car_index);
            fx_ring_clear();
            player_status_set(car_index,1);
            car_reset_state(car_index);
            frame.local_38 = (int)player_rank_key(car_index);
            course_lap_line_find_index(&frame.local_38);
            grid_position(frame.local_38,0,1,&frame.local_2c,&frame.local_34);
            car_spawn(car_index,frame.local_2c,frame.local_28,frame.local_24,frame.local_34,&g_car_801b0c);
            respawn_count = respawn_count + -1;
          }
          if (game_mode == 4) {
            frame.local_10[car_index] = 0;
            frame.local_20[car_index] = 0;
            car_set_mode(car_index,0,0);
            debris_slot_clear_forward(car_index);
            fx_ring_clear();
            player_status_set(car_index,1);
            car_reset_state(car_index);
            frame.local_38 = (int)player_rank_key(car_index);
            course_lap_line_find_index(&frame.local_38);
            grid_position(frame.local_38,car_index,4,&frame.local_2c,&frame.local_34);
            car_spawn(car_index,frame.local_2c,frame.local_28,frame.local_24,frame.local_34,&g_car_801b0c);
            respawn_count = respawn_count + -1;
          }
        }
      }
    }
LAB_00428da3:
    car_index = car_index + 1;
    if (3 < (int)car_index) {
      /* pass 3: if every car in a mode has hit respawn, kill the cars still flagged
         CAR_MODE_RESPAWN and update the battle mode (inferred) */
      if ((race_phase != 0) && (race_phase == respawn_count)) {
        active_count = 0;
        respawn_count = 0;
        do {
          timer = race_setup_slot_used(respawn_count);
          if (timer != 0) {
            timer = car_is_active(respawn_count);
            if ((timer == 1) && (frame.local_20[respawn_count] == CAR_MODE_RESPAWN)) {
              car_kill(respawn_count);
              active_count = respawn_count + 1;
              if (1 < race_phase) {
                player_add_points(respawn_count,0);
              }
            }
          }
          respawn_count = respawn_count + 1;
        } while (respawn_count < CAR_SLOTS);
        if (active_count != 0) {
          race_battle_mode_update(active_count + -1,race_phase == 1);
        }
      }
      return 1;
    }
  } while( true );
}


