// cluster-entries: race_battle_mode_update
/* Installed cluster race_battle_mode_update (tools/cluster_install.py from output/cluster_pilot_kg/race_battle_mode_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00409eb0*/ extern double player_rank_key();
/*00409ed0*/ extern int player_set_precise_position();
/*00409f30*/ extern double player_progress_b();
/*0040c820*/ extern int player_add_points();
/*0040cc60*/ extern int player_get_points();
/*0040f090*/ extern undefined __cdecl player_progress_difference(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*0040ff20*/ extern int race_setup_get_slot_type();
/*00426b40*/ extern int car_kill();
/*00434a10*/ extern int is_team_mode();
/*00434bb0*/ extern int get_event_type();
/*004379b0*/ extern int menu_bot_skill_a_get();
/*0047a780*/ extern int car_add_field_b10();
/*0047a830*/ extern int car_is_active();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*005dd62c*/ extern float k_f_100;
/*005dd7f4*/ extern float k_f_80;
/*005ddea0*/ extern float k_f_20;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7c0*/ extern undefined4 g_cars_7ef7c0;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008d8b14*/ extern undefined4 g_race_setup;
/*008d8b20*/ extern undefined4 g_round_8d8b20;
/*008d8b24*/ extern undefined4 g_round_8d8b24;
/*008d8b30*/ extern undefined4 player_count;
/* race_battle_mode_update */
/* byte offsets into g_race_setup of the 4 per-car "enabled" words */
#define RACE_SETUP_CAR_ENABLED 0xb8
#define RACE_SETUP_CAR_ENABLED_END 200
/* loop limit for the PlayerSlot.state walk: slots are 0x10 bytes, state is at +4, so 4*0x10 + 4 is where the (non-existent) 5th slot's state would be */
#define PLAYER_SLOTS_STATE_LIMIT 0x44

/* player progress runs 0..100 around the lap: a car that has just crossed the line (< 20) is ahead of one that is about to (> 80).  Puts the two on a common scale
   (the one that wrapped is moved down by a lap) so that `a <= b` means car a is behind car b.  The first value is rounded to float, the second stays extended like in the original. */
static void progress_unwrapped(int car_a, int car_b, float10 *a, float10 *b)
{
  float10 progress_a;
  float10 progress_b;

  progress_a = (float10)player_progress_b(car_a);
  progress_b = (float10)player_progress_b(car_b);
  progress_a = (float10)(float)progress_a;
  if (!(progress_a > (float10)k_f_80) || !(progress_b < (float10)k_f_20)) {      /* NaN takes this branch (test ah,0x41; jne / test ah,5; jp) */
    if (((float10)k_f_80 < progress_b) && (progress_a < (float10)k_f_20)) {
      progress_b = progress_b - (float10)k_f_100;
    }
  }
  else {
    progress_a = progress_a - (float10)k_f_100;
  }
  *a = progress_a;
  *b = progress_b;
}

/* "'car reset=%d, score=%d' / 'numcarsremain': per frame scoring, resets and kills" */

/* Called when a car is destroyed (dead_car = its index, score = the value logged with it). Counts the cars
   still active (car_is_active over the enabled cars of g_race_setup), appends dead_car to the elimination
   list g_round_8d8b20, and awards points through player_add_points depending on player_count: duel (2) -1 for
   the loser and +1 for the survivor, 3 / 4 players free-for-all (points to the dead car) or team mode (+1 for
   every slot on the winning team, -1 for the others). Event type and game mode select the standard, hare-and-
   hounds or career variants, and with bots in multiplayer events 6..8 the rear bot is decided by track
   progress (progress_unwrapped) and killed. Returns 1 when a winner is known (all four precise positions are
   then set to its rank key), else 0. Inferred from the callees, constants and debug strings; the exact
   scoring rules per mode are not verified. */
/* Locals that could not be split without changing the compiled code (register allocation): car_idx = car index in the race-setup scan loops, the index of the current best/last candidate, the count of already finished cars; scan = byte offset into g_race_setup's car-enabled table (stride 4), the team state of the first scanned slot, a loop counter over the 4 cars, or an is_team_mode() result; cars_remaining_or_i = number of active cars, later reused as a loop counter / car index; event_type_or_off = get_event_type() result, later a byte offset. Already split off: is_duel (player_count == 2), team_state (team of the car being scored, 3 players) and team_state_c (same, 4 players). */
undefined4 race_battle_mode_update(int dead_car, int score)

{
  float rank_key;
  PlayerSlot *slot_ptr;
  int event_type_or_off;
  int tmp;
  int car_active;
  int team_state_c;
  int team_state;
  int cars_remaining_or_i;
  int scan;
  int *slot_state_ptr;
  int car_idx;
  bool first_seen;
  bool is_duel;
  float10 prog_a;
  float10 prog_b;
  undefined4 team_points;
  struct {
    int candidate;
    undefined1 pad_10[8];
    int winner;
  } frame;
  
  car_idx = 0;
  frame.winner = 0;
  event_type_or_off = get_event_type();
  FID_conflict__wprintf("car reset=%d, score=%d\n",dead_car,score);
  cars_remaining_or_i = 0;
  scan = RACE_SETUP_CAR_ENABLED;
  do {
    if ((*(int *)(scan + g_race_setup) != 0) && (tmp = car_is_active(car_idx), tmp == 1)) {
      cars_remaining_or_i = cars_remaining_or_i + 1;
    }
    scan = scan + 4;
    car_idx = car_idx + 1;
  } while (scan < RACE_SETUP_CAR_ENABLED_END);
  FID_conflict__wprintf("numcarsremaining=%d\n",cars_remaining_or_i);
  if (cars_remaining_or_i == 1) {
    car_idx = 0;
    scan = RACE_SETUP_CAR_ENABLED;
    do {
      if ((*(int *)(scan + g_race_setup) != 0) && (tmp = car_is_active(car_idx), tmp == 1)) {
        frame.winner = car_idx;
      }
      scan = scan + 4;
      car_idx = car_idx + 1;
    } while (scan < RACE_SETUP_CAR_ENABLED_END);
  }
  car_idx = 0;
  scan = g_round_8d8b20;
  while (scan != -1) {
    slot_state_ptr = &g_round_8d8b24 + car_idx;
    car_idx = car_idx + 1;
    scan = *slot_state_ptr;
  }
  /* player_count == 2 (duel): the loser gets -1, the survivor +1 when one car remains */
  is_duel = player_count == 2;
  (&g_round_8d8b20)[car_idx] = dead_car;
  if (is_duel) {

    if (cars_remaining_or_i == 1) {
      FID_conflict__wprintf("loser is %d, winner is %d\n",dead_car,frame.winner);
      player_add_points(dead_car,0xffffffff);
      player_add_points(frame.winner,1);
    }
    else {
      player_add_points(0,0);
      player_add_points(1,0);
    }
  }
  frame.candidate = car_idx;
  /* 3 players: free-for-all (points to the dead car) or team mode (every slot on the winning team gets +1, the others -1) */
  if (player_count == 3) {
    scan = is_team_mode();
    if (scan == 0) {
      if (cars_remaining_or_i == 2) {
        player_add_points(dead_car,-score);
        if (game_mode == GAME_MODE_HARE_AND_HOUNDS) {
          first_seen = false;
          scan = 0;
          do {
            if ((g_cars_7ef7c0 != scan) && (tmp = car_is_active(scan), tmp == 1)) {
              if (first_seen) {
                car_kill(scan);
                player_add_points(scan,0);
                player_add_points(car_idx,0);
              }
              else {
                first_seen = true;
                car_idx = scan;
              }
            }
            scan = scan + 1;
            frame.candidate = car_idx;
          } while (scan < 4);
        }
        if (((((((event_type_or_off == EVENT_CUP_3) || (event_type_or_off == EVENT_CUP_4)) || (event_type_or_off == EVENT_CUP_5)) ||
              ((event_type_or_off == EVENT_TURBO_TOURNAMENT || (event_type_or_off == EVENT_ICE_TOURNAMENT)))) || (event_type_or_off == EVENT_NIGHT_TOURNAMENT)) ||
            (((event_type_or_off == EVENT_QUICK_RACE_11 || (event_type_or_off == EVENT_QUICK_RACE_12)) || (event_type_or_off == EVENT_QUICK_RACE_13)))) &&
           ((scan = is_team_mode(), scan == 0 && (game_mode == GAME_MODE_STANDARD)))) {
          first_seen = false;
          scan = 0;
          car_idx = RACE_SETUP_CAR_ENABLED;
          do {
            if (((*(int *)(car_idx + g_race_setup) != 0) && (tmp = car_is_active(scan), tmp == 1)
                ) && (tmp = race_setup_get_slot_type(scan), tmp == 2)) {
              if (first_seen) {
                progress_unwrapped(scan, frame.candidate, &prog_a, &prog_b);
                if (!(prog_a > prog_b)) {
                  car_kill(scan);
                  player_add_points(frame.candidate,score);
                  tmp = scan;
                }
                else {
                  car_kill(frame.candidate);
                  player_add_points(scan,score);
                  tmp = frame.candidate;
                }
                player_add_points(tmp,0);
              }
              else {
                first_seen = true;
                frame.candidate = scan;
              }
            }
            car_idx = car_idx + 4;
            scan = scan + 1;
          } while (car_idx < RACE_SETUP_CAR_ENABLED_END);
        }
      }
      else if (cars_remaining_or_i == 1) {
        player_add_points(dead_car,0);
        player_add_points(frame.winner,1);
      }
    }
    else if (cars_remaining_or_i == 2) {
      scan = -1;
      tmp = 0;
      do {
        if ((*(int *)(g_race_setup + RACE_SETUP_CAR_ENABLED + tmp * 4) != 0) &&
           (car_active = car_is_active(tmp), car_active == 1)) {
          if (scan == -1) {
            scan = g_player_slots[tmp].state;
            car_idx = tmp;
            frame.candidate = tmp;
          }
          else {
            slot_ptr = g_player_slots + tmp;
            if (scan == g_player_slots[tmp].state) {
              progress_unwrapped(tmp, car_idx, &prog_a, &prog_b);
              if (!(prog_a > prog_b)) {
                car_kill(tmp);
              }
              else {
                car_kill(car_idx);
                tmp = car_idx;
              }
              car_add_field_b10(tmp,1);
              team_state = slot_ptr->state;
              tmp = 0;
              slot_state_ptr = &g_player_slots[0].state;
              do {
                if (((PlayerSlot *)(slot_state_ptr + -1))->controller != -1) {
                  if (*slot_state_ptr == team_state) {
                    team_points = 1;
                  }
                  else {
                    team_points = 0xffffffff;
                  }
                  player_add_points(tmp,team_points);
                }
                slot_state_ptr = slot_state_ptr + 4;
                tmp = tmp + 1;
                car_idx = frame.candidate;
              } while ((int)slot_state_ptr < (int)&g_player_slots + PLAYER_SLOTS_STATE_LIMIT);
            }
          }
        }
        tmp = tmp + 1;
      } while (tmp < 4);
    }
    else if (cars_remaining_or_i == 1) {
      scan = g_player_slots[frame.winner].state;
      car_idx = 0;
      slot_state_ptr = &g_player_slots[0].state;
      do {
        if (((PlayerSlot *)(slot_state_ptr + -1))->controller != -1) {
          if (*slot_state_ptr == scan) {
            team_points = 1;
          }
          else {
            team_points = 0xffffffff;
          }
          player_add_points(car_idx,team_points);
        }
        slot_state_ptr = slot_state_ptr + 4;
        car_idx = car_idx + 1;
      } while ((int)slot_state_ptr < (int)&g_player_slots + PLAYER_SLOTS_STATE_LIMIT);
    }
  }
  /* 4 players: same scheme; with bots (menu_bot_skill_a) in multiplayer events 6..8 the two remaining bots are decided by progress, the rear one is killed */
  if (player_count == 4) {
    scan = get_event_type();
    if ((((scan == EVENT_MULTIPLAYER_6) || (scan = get_event_type(), scan == EVENT_MULTIPLAYER_7)) ||
        (scan = get_event_type(), scan == EVENT_MULTIPLAYER_8)) &&
       ((scan = menu_bot_skill_a_get(), scan != 0 &&
        (FID_conflict__wprintf("here, numremaining=%d\n",cars_remaining_or_i), cars_remaining_or_i == 2)))) {
      first_seen = false;
      scan = 0;
      car_idx = RACE_SETUP_CAR_ENABLED;
      do {
        if (((*(int *)(car_idx + g_race_setup) != 0) && (tmp = car_is_active(scan), tmp == 1)) &&
           (tmp = race_setup_get_slot_type(scan), tmp == 2)) {
          if (first_seen) {
            progress_unwrapped(scan, frame.candidate, &prog_a, &prog_b);
            if (!(prog_a > prog_b)) {
              car_kill(scan);
              player_add_points(frame.candidate,2);
              tmp = scan;
            }
            else {
              car_kill(frame.candidate);
              player_add_points(scan,2);
              tmp = frame.candidate;
            }
            player_add_points(tmp,1);
          }
          else {
            first_seen = true;
            frame.candidate = scan;
          }
        }
        car_idx = car_idx + 4;
        scan = scan + 1;
      } while (car_idx < RACE_SETUP_CAR_ENABLED_END);
    }
    scan = is_team_mode();
    if (scan == 0) {
      if (cars_remaining_or_i == 3) {
        player_add_points(dead_car,score * -2);
        if (game_mode == GAME_MODE_HARE_AND_HOUNDS) {
          cars_remaining_or_i = 0;
          scan = 0;
          do {
            if ((g_cars_7ef7c0 != scan) && (car_idx = car_is_active(scan), car_idx == 1)) {
              if (cars_remaining_or_i == 0) {
                cars_remaining_or_i = 1;
                frame.candidate = scan;
              }
              else if (cars_remaining_or_i == 1) {
                cars_remaining_or_i = 2;
                dead_car = scan;
              }
              else if (cars_remaining_or_i == 2) {
                car_kill(scan);
                player_add_points(scan,0);
                car_kill(frame.candidate);
                player_add_points(frame.candidate,0);
                player_add_points(dead_car,0);
              }
            }
            scan = scan + 1;
          } while (scan < 4);
        }
        if (((((event_type_or_off == EVENT_CUP_3) || (event_type_or_off == EVENT_CUP_4)) || (event_type_or_off == EVENT_CUP_5)) ||
            ((((event_type_or_off == EVENT_TURBO_TOURNAMENT || (event_type_or_off == EVENT_ICE_TOURNAMENT)) ||
              ((event_type_or_off == EVENT_NIGHT_TOURNAMENT || ((event_type_or_off == EVENT_QUICK_RACE_11 || (event_type_or_off == EVENT_QUICK_RACE_12)))))) || (event_type_or_off == EVENT_QUICK_RACE_13)))) &&
           ((event_type_or_off = is_team_mode(), event_type_or_off == 0 && (game_mode == GAME_MODE_STANDARD)))) {
          scan = 0;
          cars_remaining_or_i = 0;
          event_type_or_off = RACE_SETUP_CAR_ENABLED;
          do {
            if (((*(int *)(event_type_or_off + g_race_setup) != 0) && (car_idx = car_is_active(cars_remaining_or_i), car_idx == 1)
                ) && (car_idx = race_setup_get_slot_type(cars_remaining_or_i), car_idx == 2)) {
              if (scan == 0) {
                scan = 1;
                frame.candidate = cars_remaining_or_i;
              }
              else if (scan == 1) {
                scan = 2;
                dead_car = cars_remaining_or_i;
              }
              else if (scan == 2) {
                player_progress_difference(cars_remaining_or_i,frame.candidate,dead_car,score);
              }
            }
            event_type_or_off = event_type_or_off + 4;
            cars_remaining_or_i = cars_remaining_or_i + 1;
          } while (event_type_or_off < RACE_SETUP_CAR_ENABLED_END);
          return 0;
        }
      }
      else if (cars_remaining_or_i == 2) {
        player_add_points(dead_car,-score);
        if (game_mode == GAME_MODE_HARE_AND_HOUNDS) {
          first_seen = false;
          cars_remaining_or_i = 0;
          do {
            if ((g_cars_7ef7c0 != cars_remaining_or_i) && (scan = car_is_active(cars_remaining_or_i), scan == 1)) {
              if (first_seen) {
                car_kill(cars_remaining_or_i);
                player_add_points(cars_remaining_or_i,0);
                player_add_points(frame.candidate,0);
              }
              else {
                first_seen = true;
                frame.candidate = cars_remaining_or_i;
              }
            }
            cars_remaining_or_i = cars_remaining_or_i + 1;
          } while (cars_remaining_or_i < 4);
        }
        if (((((((event_type_or_off == EVENT_CUP_3) || (event_type_or_off == EVENT_CUP_4)) || (event_type_or_off == EVENT_CUP_5)) ||
              ((event_type_or_off == EVENT_TURBO_TOURNAMENT || (event_type_or_off == EVENT_ICE_TOURNAMENT)))) ||
             ((event_type_or_off == EVENT_NIGHT_TOURNAMENT || ((event_type_or_off == EVENT_QUICK_RACE_11 || (event_type_or_off == EVENT_QUICK_RACE_12)))))) || (event_type_or_off == EVENT_QUICK_RACE_13)) &&
           ((event_type_or_off = is_team_mode(), event_type_or_off == 0 && (game_mode == GAME_MODE_STANDARD)))) {
          first_seen = false;
          event_type_or_off = 0;
          cars_remaining_or_i = RACE_SETUP_CAR_ENABLED;
          do {
            if (((*(int *)(cars_remaining_or_i + g_race_setup) != 0) && (scan = car_is_active(event_type_or_off), scan == 1)
                ) && (scan = race_setup_get_slot_type(event_type_or_off), scan == 2)) {
              if (first_seen) {
                progress_unwrapped(event_type_or_off, frame.candidate, &prog_a, &prog_b);
                if (!(prog_a > prog_b)) {
                  car_kill(event_type_or_off);
                  player_add_points(frame.candidate,2);
                  scan = event_type_or_off;
                }
                else {
                  car_kill(frame.candidate);
                  player_add_points(event_type_or_off,2);
                  scan = frame.candidate;
                }
                player_add_points(scan,1);
              }
              else {
                first_seen = true;
                frame.candidate = event_type_or_off;
              }
            }
            cars_remaining_or_i = cars_remaining_or_i + 4;
            event_type_or_off = event_type_or_off + 1;
          } while (cars_remaining_or_i < RACE_SETUP_CAR_ENABLED_END);
          return 0;
        }
      }
      else if (cars_remaining_or_i == 1) {
        event_type_or_off = player_get_points(dead_car);
        if (10 < event_type_or_off) {
          score = 0;
        }
        player_add_points(dead_car,score);
        player_add_points(frame.winner,2);
        goto LAB_00411c7e;
      }
    }
    else if (cars_remaining_or_i == 3) {
      event_type_or_off = 0;
      frame.winner = -1;
      cars_remaining_or_i = 0;
      do {
        if ((*(int *)(g_race_setup + RACE_SETUP_CAR_ENABLED + cars_remaining_or_i * 4) != 0) &&
           (scan = car_is_active(cars_remaining_or_i), scan == 1)) {
          if (frame.winner == -1) {
            frame.winner = g_player_slots[cars_remaining_or_i].state;
            event_type_or_off = event_type_or_off + 1;
            frame.candidate = cars_remaining_or_i;
          }
          else if (frame.winner == g_player_slots[cars_remaining_or_i].state) {
            if (event_type_or_off == 1) {
              dead_car = cars_remaining_or_i;
            }
            event_type_or_off = event_type_or_off + 1;
            if (event_type_or_off == 3) {
              progress_unwrapped(cars_remaining_or_i, frame.candidate, &prog_a, &prog_b);
              if (!(prog_a > prog_b)) {
                car_kill(cars_remaining_or_i);
                progress_unwrapped(frame.candidate, dead_car, &prog_a, &prog_b);
                scan = dead_car;
                car_idx = frame.candidate;
                if (prog_b < prog_a) goto LAB_00411724;
              }
              else {
                car_kill(frame.candidate);
                progress_unwrapped(cars_remaining_or_i, dead_car, &prog_a, &prog_b);
                scan = cars_remaining_or_i;
                car_idx = dead_car;
                if (!(prog_a > prog_b)) {
LAB_00411724:
                  car_idx = scan;
                }
              }
              car_kill(car_idx);
              scan = g_player_slots[cars_remaining_or_i].state;
              cars_remaining_or_i = 0;
              slot_state_ptr = &g_player_slots[0].state;
              do {
                if (((PlayerSlot *)(slot_state_ptr + -1))->controller != -1) {
                  car_idx = score;
                  if (*slot_state_ptr != scan) {
                    car_idx = -score;
                  }
                  player_add_points(cars_remaining_or_i,car_idx);
                }
                slot_state_ptr = slot_state_ptr + 4;
                cars_remaining_or_i = cars_remaining_or_i + 1;
              } while ((int)slot_state_ptr < (int)&g_player_slots + PLAYER_SLOTS_STATE_LIMIT);
            }
          }
        }
        cars_remaining_or_i = cars_remaining_or_i + 1;
      } while (cars_remaining_or_i < 4);
    }
    else {
      if (cars_remaining_or_i == 2) {
        event_type_or_off = -1;
        scan = 0;
        cars_remaining_or_i = frame.candidate;
        do {
          if ((*(int *)(g_race_setup + RACE_SETUP_CAR_ENABLED + scan * 4) != 0) &&
             (car_idx = car_is_active(scan), car_idx == 1)) {
            if (event_type_or_off == -1) {
              event_type_or_off = g_player_slots[scan].state;
              cars_remaining_or_i = scan;
              frame.candidate = scan;
            }
            else {
              slot_ptr = g_player_slots + scan;
              if (event_type_or_off == g_player_slots[scan].state) {
                progress_unwrapped(scan, cars_remaining_or_i, &prog_a, &prog_b);
                if (prog_b < prog_a) {
                  scan = cars_remaining_or_i;
                }
                car_kill(scan);
                team_state_c = slot_ptr->state;
                scan = 0;
                slot_state_ptr = &g_player_slots[0].state;
                do {
                  if (((PlayerSlot *)(slot_state_ptr + -1))->controller != -1) {
                    if (*slot_state_ptr == team_state_c) {
                      team_points = 1;
                    }
                    else {
                      team_points = 0xffffffff;
                    }
                    player_add_points(scan,team_points);
                  }
                  slot_state_ptr = slot_state_ptr + 4;
                  scan = scan + 1;
                  cars_remaining_or_i = frame.candidate;
                } while ((int)slot_state_ptr < (int)&g_player_slots + PLAYER_SLOTS_STATE_LIMIT);
              }
            }
          }
          scan = scan + 1;
        } while (scan < 4);
        return 0;
      }
      if (cars_remaining_or_i == 1) {
        event_type_or_off = g_player_slots[frame.winner].state;
        cars_remaining_or_i = 0;
        slot_state_ptr = &g_player_slots[0].state;
        do {
          if (((PlayerSlot *)(slot_state_ptr + -1))->controller != -1) {
            if (*slot_state_ptr == event_type_or_off) {
              team_points = 1;
            }
            else {
              team_points = 0xffffffff;
            }
            player_add_points(cars_remaining_or_i,team_points);
          }
          slot_state_ptr = slot_state_ptr + 4;
          cars_remaining_or_i = cars_remaining_or_i + 1;
        } while ((int)slot_state_ptr < (int)&g_player_slots + PLAYER_SLOTS_STATE_LIMIT);
        goto LAB_00411c7e;
      }
    }
  }
  else if (cars_remaining_or_i == 1) {
/* a winner is known: all four precise positions are set to its rank key */
LAB_00411c7e:
    prog_a = (float10)player_rank_key(frame.winner);
    rank_key = (float)prog_a;
    player_set_precise_position(0,*(int *)&rank_key);
    player_set_precise_position(1,*(int *)&rank_key);
    player_set_precise_position(2,*(int *)&rank_key);
    player_set_precise_position(3,*(int *)&rank_key);
    return 1;
  }
  return 0;
}

