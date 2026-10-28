// cluster-entries: race_spawn_cars
/* Installed cluster race_spawn_cars (tools/cluster_install.py from output/cluster_pilot_j1/race_spawn_cars.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#define GRID_ORDER_UNSET 1000 /* no grid slot assigned yet to this player */
/*00409ed0*/ extern int player_set_precise_position();
/*00409f60*/ extern int grid_position();
/*0040a830*/ extern int course_lap_line_find_index();
/*0040c7e0*/ extern int round_scores_reset();
/*0040c9a0*/ extern int player_get_points_change();
/*0040fe50*/ extern int team_color_rgba();
/*00421e60*/ extern int vehicle_set_flag_10();
/*00421ed0*/ extern int vehicle_set_flag_1000();
/*00422010*/ extern int vehicle_set_flag_400();
/*00422090*/ extern int vehicle_get_matrix();
/*0042e940*/ extern int course_banner_state_clear();
/*00434a10*/ extern int is_team_mode();
/*00434bb0*/ extern int get_event_type();
/*00435f10*/ extern int get_career_row();
/*00437f10*/ extern int training_lesson_get();
/*00437f20*/ extern int ui_get_car_8c86c8();
/*00467890*/ extern int player_status_set();
/*004797b0*/ extern int car_set_bounds();
/*00479b30*/ extern int car_init_dynamics();
/*0047a100*/ extern int car_reset_state();
/*0047a750*/ extern int car_get_field_b10();
/*0047b520*/ extern int car_current_matrix();
/*0047eb50*/ extern undefined __cdecl car_spawn(uint p0, RwReal p1, float p2, RwReal p3, RwReal p4, undefined4 p5);
/*0047fe10*/ extern int hud_labels_clear();
/*00652e98*/ extern undefined4 g_race_spawn_652e98;
/*00652f5c*/ extern undefined4 g_race_652f5c;
/*007eeb84*/ extern undefined4 g_race_7eeb84[0x1000];
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7c0*/ extern undefined4 g_cars_7ef7c0;
/*007ef7e0*/ extern undefined4 g_race_clock_b;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*00801b0c*/ extern undefined4 g_car_801b0c;
/*008d8b10*/ extern undefined4 g_race_spawn_8d8b10;
/*008d8b14*/ extern undefined4 g_race_setup;
/*008d8b20*/ extern undefined4 g_round_8d8b20;
/*008d8b30*/ extern undefined4 player_count;
/* race_spawn_cars */

/* "sets up the cars of a race: car_set_bounds, car_init_dynamics, car_reset_state, grid_position,
   car_spawn" */

/* Sets up all cars at race start (no parameters). For every active player slot it sets the car bounds and
   dynamics (car_set_bounds, car_init_dynamics), resets the car state, picks the start grid slot
   (grid_position; the order comes from player_get_points_change, so the previous standings decide who starts
   where; 1000 = unset), spawns the car at that grid position with its vehicle flags (vehicle_set_flag_*),
   assigns team colours in team mode and clears the per-race HUD labels, banner state and round scores. In
   training events the lesson number (training_lesson_get) selects a special start setup (g_race_652f5c, start slot). Inferred from the callees; flag numbers are
   named by value only. */
void race_spawn_cars(void)

{
  float fbits1;
  uint tmp_a;
  uint order2_or_i;
  uint tmp_b;
  int event_type;
  uint order1_or_slot;
  PlayerSlot *slot_ptr;
  uint order0_or_player;
  undefined4 *race_rec_ptr;
  uint order3;
  int idx_or_team_mode;
  bool placed;
  byte clash_count;
  struct {
    uint points_change_or_tmp;
    int all_unchanged;
    RwReal heading;
    undefined1 car_matrix_ptr [4];
    undefined1 team_rgba [4];
    RwReal spawn_x;
    float spawn_y;
    RwReal spawn_z;
    uint grid_order [10];
  } frame;
  int ctrl_or_grid_cars;
  
  g_race_spawn_8d8b10 = g_race_spawn_8d8b10 + 1;
  g_race_clock_b = 0;
  if (0xb < g_race_spawn_8d8b10) {
    g_race_spawn_8d8b10 = 0;
  }
  hud_labels_clear();
  course_banner_state_clear();
  order0_or_player = GRID_ORDER_UNSET;
  idx_or_team_mode = 0;
  frame.all_unchanged = 1;
  race_rec_ptr = g_race_7eeb84;
  /* grid_order[0..3]: grid slot offset per player (1000 = unset), derived from player_get_points_change; [4..7] are reused as scratch */
  frame.grid_order[3] = GRID_ORDER_UNSET;
  frame.grid_order[0] = GRID_ORDER_UNSET;
  frame.grid_order[1] = GRID_ORDER_UNSET;
  frame.grid_order[2] = GRID_ORDER_UNSET;
  slot_ptr = g_player_slots;
  do {
    ctrl_or_grid_cars = slot_ptr->controller;
    race_rec_ptr[-1] = 0;
    *race_rec_ptr = 0;
    race_rec_ptr[1] = 0;
    if (-1 < ctrl_or_grid_cars) {
      vehicle_set_flag_400(idx_or_team_mode,0);
      vehicle_set_flag_10(idx_or_team_mode,0,0);
      vehicle_set_flag_1000(idx_or_team_mode,0,0);
    }
    race_rec_ptr = race_rec_ptr + 3;
    idx_or_team_mode = idx_or_team_mode + 1;
    slot_ptr = slot_ptr + 1;
  } while ((int)race_rec_ptr < (int)g_race_7eeb84 + 0x30);
  idx_or_team_mode = 0;
  order1_or_slot = frame.grid_order[1];
  do {
    frame.points_change_or_tmp = player_get_points_change(idx_or_team_mode);
    switch(player_count) {
    case 1:
switchD_0041048e_caseD_1:
      frame.grid_order[idx_or_team_mode] = 0;
      order1_or_slot = frame.grid_order[1];
      order0_or_player = frame.grid_order[0];
      break;
    case 2:
      if (frame.points_change_or_tmp == 1) {
        frame.grid_order[idx_or_team_mode] = 1;
        order1_or_slot = frame.grid_order[1];
        order0_or_player = frame.grid_order[0];
      }
      else {
LAB_004104bd:
        placed = frame.points_change_or_tmp == 0xffffffff;
LAB_004104f2:
        if (placed) goto switchD_0041048e_caseD_1;
      }
      break;
    case 3:
      if (frame.points_change_or_tmp == 1) {
        frame.grid_order[idx_or_team_mode] = 2;
        order1_or_slot = frame.grid_order[1];
        order0_or_player = frame.grid_order[0];
      }
      else {
        if (frame.points_change_or_tmp != 0) goto LAB_004104bd;
        frame.grid_order[idx_or_team_mode] = 1;
        order1_or_slot = frame.grid_order[1];
        order0_or_player = frame.grid_order[0];
      }
      break;
    case 4:
      if (frame.points_change_or_tmp == 2) {
        frame.grid_order[idx_or_team_mode] = 3;
        order1_or_slot = frame.grid_order[1];
        order0_or_player = frame.grid_order[0];
      }
      else if (frame.points_change_or_tmp == 1) {
        frame.grid_order[idx_or_team_mode] = 2;
        order1_or_slot = frame.grid_order[1];
        order0_or_player = frame.grid_order[0];
      }
      else {
        if (frame.points_change_or_tmp != 0xffffffff) {
          placed = frame.points_change_or_tmp == 0xfffffffe;
          goto LAB_004104f2;
        }
        frame.grid_order[idx_or_team_mode] = 1;
        order1_or_slot = frame.grid_order[1];
        order0_or_player = frame.grid_order[0];
      }
    }
    idx_or_team_mode = idx_or_team_mode + 1;
    if (3 < idx_or_team_mode) {
      if (order0_or_player != GRID_ORDER_UNSET) {
        frame.all_unchanged = 0;
      }
      if (order1_or_slot != GRID_ORDER_UNSET) {
        frame.all_unchanged = 0;
      }
      if (frame.grid_order[2] != GRID_ORDER_UNSET) {
        frame.all_unchanged = 0;
      }
      if (frame.grid_order[3] != GRID_ORDER_UNSET) {
        frame.all_unchanged = 0;
      }
      idx_or_team_mode = 0;
      do {
        frame.points_change_or_tmp = player_get_points_change(idx_or_team_mode);
        if (player_count == 2) {
          if ((frame.points_change_or_tmp != 1) && (frame.points_change_or_tmp != 0xffffffff)) {
            placed = false;
            order2_or_i = 0;
            do {
              ctrl_or_grid_cars = -1;
              if (order0_or_player == order2_or_i) {
                ctrl_or_grid_cars = 0;
              }
              if ((order1_or_slot == order2_or_i) && (ctrl_or_grid_cars == -1)) {
                ctrl_or_grid_cars = 1;
              }
              if ((frame.grid_order[2] == order2_or_i) && (ctrl_or_grid_cars == -1)) {
                ctrl_or_grid_cars = 2;
              }
              if (((frame.grid_order[3] != order2_or_i) && (ctrl_or_grid_cars == -1)) && (!placed)) {
                frame.grid_order[idx_or_team_mode] = order2_or_i;
                placed = true;
                order0_or_player = frame.grid_order[0];
              }
              order2_or_i = order2_or_i + 1;
              order1_or_slot = frame.grid_order[1];
            } while ((int)order2_or_i < 4);
          }
        }
        else if (player_count == 3) {
          if (((frame.points_change_or_tmp != 1) && (frame.points_change_or_tmp != 0)) && (frame.points_change_or_tmp != 0xffffffff)) {
            placed = false;
            order2_or_i = 0;
            do {
              ctrl_or_grid_cars = -1;
              if (order0_or_player == order2_or_i) {
                ctrl_or_grid_cars = 0;
              }
              if ((order1_or_slot == order2_or_i) && (ctrl_or_grid_cars == -1)) {
                ctrl_or_grid_cars = 1;
              }
              if ((frame.grid_order[2] == order2_or_i) && (ctrl_or_grid_cars == -1)) {
                ctrl_or_grid_cars = 2;
              }
              if (((frame.grid_order[3] != order2_or_i) && (ctrl_or_grid_cars == -1)) && (!placed)) {
                frame.grid_order[idx_or_team_mode] = order2_or_i;
                placed = true;
                order0_or_player = frame.grid_order[0];
              }
              order2_or_i = order2_or_i + 1;
              order1_or_slot = frame.grid_order[1];
            } while ((int)order2_or_i < 4);
          }
        }
        else if (((player_count == 4) && (frame.points_change_or_tmp != 2)) &&
                ((frame.points_change_or_tmp != 1 && ((frame.points_change_or_tmp != 0xffffffff && (frame.points_change_or_tmp != 0xfffffffe)))))) {
          placed = false;
          order2_or_i = 0;
          do {
            ctrl_or_grid_cars = -1;
            if (order0_or_player == order2_or_i) {
              ctrl_or_grid_cars = 0;
            }
            if ((order1_or_slot == order2_or_i) && (ctrl_or_grid_cars == -1)) {
              ctrl_or_grid_cars = 1;
            }
            if ((frame.grid_order[2] == order2_or_i) && (ctrl_or_grid_cars == -1)) {
              ctrl_or_grid_cars = 2;
            }
            if (((frame.grid_order[3] != order2_or_i) && (ctrl_or_grid_cars == -1)) && (!placed)) {
              frame.grid_order[idx_or_team_mode] = order2_or_i;
              placed = true;
              order0_or_player = frame.grid_order[0];
            }
            order2_or_i = order2_or_i + 1;
            order1_or_slot = frame.grid_order[1];
          } while ((int)order2_or_i < 4);
        }
        idx_or_team_mode = idx_or_team_mode + 1;
      } while (idx_or_team_mode < 4);
      if (player_count == 2) {
        while( true ) {
          while( true ) {
            clash_count = order0_or_player == 0;
            if (order1_or_slot == 0) {
              clash_count = clash_count + 1;
            }
            if (clash_count < 2) break;
            if (order0_or_player == 0) {
              order0_or_player = 1;
            }
            else if (order1_or_slot == 0) {
              order1_or_slot = 1;
            }
          }
          frame.points_change_or_tmp = (uint)(order0_or_player == 1);
          if (order1_or_slot == 1) {
            frame.points_change_or_tmp = frame.points_change_or_tmp + 1;
          }
          frame.grid_order[0] = order0_or_player;
          frame.grid_order[1] = order1_or_slot;
          if (frame.points_change_or_tmp < 2) break;
          if (order0_or_player == 1) {
            order0_or_player = 0;
          }
          else if (order1_or_slot == 1) {
            order1_or_slot = 0;
          }
        }
      }
      else if (player_count == 3) {
        while( true ) {
          while( true ) {
            clash_count = order0_or_player == 0;
            if (order1_or_slot == 0) {
              clash_count = clash_count + 1;
            }
            if (frame.grid_order[2] == 0) {
              clash_count = clash_count + 1;
            }
            if (clash_count < 2) break;
            if (order0_or_player == 0) {
              order0_or_player = 1;
            }
            else if (order1_or_slot == 0) {
              order1_or_slot = 1;
            }
            else if (frame.grid_order[2] == 0) {
              frame.grid_order[2] = 1;
            }
          }
          while( true ) {
            clash_count = order0_or_player == 1;
            if (order1_or_slot == 1) {
              clash_count = clash_count + 1;
            }
            if (frame.grid_order[2] == 1) {
              clash_count = clash_count + 1;
            }
            if (clash_count < 2) break;
            if (order0_or_player == 1) {
              order0_or_player = 2;
            }
            else if (order1_or_slot == 1) {
              order1_or_slot = 2;
            }
            else if (frame.grid_order[2] == 1) {
              frame.grid_order[2] = 2;
            }
          }
          frame.points_change_or_tmp = (uint)(order0_or_player == 2);
          if (order1_or_slot == 2) {
            frame.points_change_or_tmp = (order0_or_player == 2) + 1;
          }
          if (frame.grid_order[2] == 2) {
            frame.points_change_or_tmp = frame.points_change_or_tmp + 1;
          }
          frame.grid_order[0] = order0_or_player;
          frame.grid_order[1] = order1_or_slot;
          if (frame.points_change_or_tmp < 2) break;
          if (order0_or_player == 2) {
            order0_or_player = 0;
          }
          else if (order1_or_slot == 2) {
            order1_or_slot = 0;
          }
          else if (frame.grid_order[2] == 2) {
            frame.grid_order[2] = 0;
          }
        }
      }
      else if (player_count == 4) {
        order2_or_i = frame.grid_order[3];
        while( true ) {
          while( true ) {
            frame.grid_order[0] = order0_or_player;
            frame.grid_order[1] = order1_or_slot;
            clash_count = frame.grid_order[0] == 0;
            if (frame.grid_order[1] == 0) {
              clash_count = clash_count + 1;
            }
            if (frame.grid_order[2] == 0) {
              clash_count = clash_count + 1;
            }
            if (frame.grid_order[3] == 0) {
              clash_count = clash_count + 1;
            }
            if (clash_count < 2) break;
            order1_or_slot = frame.grid_order[1];
            order0_or_player = 1;
            if ((frame.grid_order[0] != 0) && (order1_or_slot = 1, order0_or_player = frame.grid_order[0], frame.grid_order[1] != 0)) {
              if (frame.grid_order[2] == 0) {
                frame.grid_order[2] = 1;
                order1_or_slot = frame.grid_order[1];
              }
              else {
                order1_or_slot = frame.grid_order[1];
                if (order2_or_i == 0) {
                  order2_or_i = 1;
                  frame.grid_order[3] = 1;
                }
              }
            }
          }
          while( true ) {
            clash_count = frame.grid_order[0] == 1;
            if (frame.grid_order[1] == 1) {
              clash_count = clash_count + 1;
            }
            if (frame.grid_order[2] == 1) {
              clash_count = clash_count + 1;
            }
            if (frame.grid_order[3] == 1) {
              clash_count = clash_count + 1;
            }
            if (clash_count < 2) break;
            if (frame.grid_order[0] == 1) {
              frame.grid_order[0] = 2;
            }
            else if (frame.grid_order[1] == 1) {
              frame.grid_order[1] = 2;
            }
            else if (frame.grid_order[2] == 1) {
              frame.grid_order[2] = 2;
            }
            else if (order2_or_i == 1) {
              order2_or_i = 2;
              frame.grid_order[3] = 2;
            }
          }
          while( true ) {
            clash_count = frame.grid_order[0] == 2;
            if (frame.grid_order[1] == 2) {
              clash_count = clash_count + 1;
            }
            if (frame.grid_order[2] == 2) {
              clash_count = clash_count + 1;
            }
            if (frame.grid_order[3] == 2) {
              clash_count = clash_count + 1;
            }
            if (clash_count < 2) break;
            if (frame.grid_order[0] == 2) {
              frame.grid_order[0] = 3;
            }
            else if (frame.grid_order[1] == 2) {
              frame.grid_order[1] = 3;
            }
            else if (frame.grid_order[2] == 2) {
              frame.grid_order[2] = 3;
            }
            else if (order2_or_i == 2) {
              order2_or_i = 3;
              frame.grid_order[3] = 3;
            }
          }
          frame.points_change_or_tmp = (uint)(frame.grid_order[0] == 3);
          if (frame.grid_order[1] == 3) {
            frame.points_change_or_tmp = (frame.grid_order[0] == 3) + 1;
          }
          if (frame.grid_order[2] == 3) {
            frame.points_change_or_tmp = frame.points_change_or_tmp + 1;
          }
          if (frame.grid_order[3] == 3) {
            frame.points_change_or_tmp = frame.points_change_or_tmp + 1;
          }
          if (frame.points_change_or_tmp < 2) break;
          order1_or_slot = frame.grid_order[1];
          if (frame.grid_order[0] == 3) {
            order0_or_player = 0;
          }
          else {
            order0_or_player = frame.grid_order[0];
            if (frame.grid_order[1] == 3) {
              order1_or_slot = 0;
            }
            else if (frame.grid_order[2] == 3) {
              frame.grid_order[2] = 0;
            }
            else if (order2_or_i == 3) {
              order2_or_i = 0;
              frame.grid_order[3] = 0;
            }
          }
        }
      }
      idx_or_team_mode = is_team_mode();
      if (idx_or_team_mode == 1) {
        if (player_count != 3) {
          order0_or_player = 0;
          frame.grid_order[4] = 0xffffffff;
          frame.grid_order[5] = 0xffffffff;
          frame.grid_order[6] = 0xffffffff;
          frame.all_unchanged = 0;
          frame.grid_order[7] = 0xffffffff;
          do {
            frame.grid_order[order0_or_player] = order0_or_player;
            order0_or_player = order0_or_player + 1;
          } while ((int)order0_or_player < 4);
          idx_or_team_mode = 0;
          if (0 < (int)player_count) {
            do {
              car_get_field_b10(idx_or_team_mode,&frame.points_change_or_tmp);
              if (frame.points_change_or_tmp == 0) {
                frame.points_change_or_tmp = 0xfffffffe;
              }
              frame.grid_order[idx_or_team_mode + 4] = frame.points_change_or_tmp;
              idx_or_team_mode = idx_or_team_mode + 1;
            } while (idx_or_team_mode < (int)player_count);
          }
          do {
            order3 = frame.grid_order[3];
            order2_or_i = frame.grid_order[2];
            order1_or_slot = frame.grid_order[1];
            order0_or_player = frame.grid_order[0];
            idx_or_team_mode = 0;
            while( true ) {
              tmp_b = frame.grid_order[idx_or_team_mode + 5];
              tmp_a = frame.grid_order[idx_or_team_mode + 4];
              if (((tmp_b < tmp_a) && (tmp_a != 0xffffffff)) && (tmp_b != 0xffffffff)) break;
              idx_or_team_mode = idx_or_team_mode + 1;
              if (2 < idx_or_team_mode) {
                tmp_b = 0;
                do {
                  if (order0_or_player == tmp_b) {
                    frame.grid_order[tmp_b] = 0;
                  }
                  if (order1_or_slot == tmp_b) {
                    frame.grid_order[tmp_b] = 1;
                  }
                  if (order2_or_i == tmp_b) {
                    frame.grid_order[tmp_b] = 2;
                  }
                  if (order3 == tmp_b) {
                    frame.grid_order[tmp_b] = 3;
                  }
                  tmp_b = tmp_b + 1;
                } while ((int)tmp_b < 4);
                goto LAB_00410b76;
              }
            }
            frame.grid_order[idx_or_team_mode + 4] = tmp_b;
            frame.grid_order[idx_or_team_mode + 5] = tmp_a;
            frame.points_change_or_tmp = frame.grid_order[idx_or_team_mode];
            frame.grid_order[idx_or_team_mode] = frame.grid_order[idx_or_team_mode + 1];
            frame.grid_order[idx_or_team_mode + 1] = frame.points_change_or_tmp;
          } while( true );
        }
        order1_or_slot = 2;
        order0_or_player = (uint)(*(int *)(g_race_setup + 0xbc) != 0);
        if (*(int *)(g_race_setup + 0xc0) == 0) {
          order0_or_player = 0;
          order1_or_slot = 1;
        }
        order2_or_i = 3;
        if (*(int *)(g_race_setup + 0xc4) == 0) {
          order0_or_player = 0;
          order1_or_slot = 1;
          order2_or_i = 2;
        }
        order3 = 0xffffffff;
        tmp_b = 0xffffffff;
        if (frame.grid_order[order0_or_player] == frame.grid_order[order1_or_slot]) {
          order3 = order0_or_player;
          tmp_b = order1_or_slot;
        }
        if (frame.grid_order[order0_or_player] == frame.grid_order[order2_or_i]) {
          order3 = order0_or_player;
          tmp_b = order2_or_i;
        }
        if (frame.grid_order[order1_or_slot] == frame.grid_order[order2_or_i]) {
          order3 = order1_or_slot;
          tmp_b = order2_or_i;
        }
        if (order3 != 0xffffffff) {
          car_get_field_b10(order3,&frame.points_change_or_tmp);
          if (frame.points_change_or_tmp == 0) {
            frame.points_change_or_tmp = 0xfffffffe;
          }
          frame.grid_order[order3 + 4] = frame.points_change_or_tmp;
          car_get_field_b10(tmp_b,&frame.points_change_or_tmp);
          if (frame.points_change_or_tmp == 0) {
            frame.points_change_or_tmp = 0xfffffffe;
          }
          frame.grid_order[tmp_b + 4] = frame.points_change_or_tmp;
          if (frame.grid_order[order3 + 4] < frame.points_change_or_tmp) {
            frame.grid_order[order3] = 1;
          }
          else {
            frame.grid_order[tmp_b] = 1;
          }
        }
      }
LAB_00410b76:
      round_scores_reset();
      idx_or_team_mode = 0;
      order0_or_player = 0;
      do {
        ctrl_or_grid_cars = g_race_setup;
        (&g_round_8d8b20)[order0_or_player] = 0xffffffff;
        if (*(int *)(ctrl_or_grid_cars + 0xb8 + order0_or_player * 4) != 0) {
          vehicle_get_matrix(order0_or_player,frame.grid_order + 4);
          car_set_bounds(order0_or_player,frame.grid_order + 4);
          car_init_dynamics(order0_or_player);
          car_reset_state(order0_or_player);
          if (player_count == 1) {
            order1_or_slot = (&g_race_spawn_652e98)[idx_or_team_mode + g_race_spawn_8d8b10 * 4];
          }
          else {
            order1_or_slot = frame.grid_order[order0_or_player];
            if (frame.all_unchanged == 1) {
              order1_or_slot = idx_or_team_mode + 4;
            }
            if (game_mode == GAME_MODE_HOLD_THE_FLAG) {
              order1_or_slot = idx_or_team_mode + 4;
            }
            else if (game_mode == GAME_MODE_HARE_AND_HOUNDS) {
              order1_or_slot = idx_or_team_mode + 4;
              if (idx_or_team_mode == g_cars_7ef7c0) {
                order1_or_slot = 100;
              }
            }
            else {
              if (game_mode == GAME_MODE_RACE) {
                order1_or_slot = 8 - idx_or_team_mode;
                ctrl_or_grid_cars = get_career_row();
                if (ctrl_or_grid_cars == 4) {
                  if (order1_or_slot == 8) {
                    order1_or_slot = 7;
                  }
                  else if (order1_or_slot == 7) {
                    order1_or_slot = 8;
                  }
                }
              }
              if (game_mode == GAME_MODE_CHASE_FUGITIVE) {
                order1_or_slot = (-(uint)(order0_or_player != 0) & 3) + 7;
              }
              else if (game_mode == GAME_MODE_SURVIVAL) {
                order1_or_slot = 8 - idx_or_team_mode;
              }
            }
          }
          course_lap_line_find_index(&g_race_652f5c);
          ctrl_or_grid_cars = player_count;
          if (game_mode == GAME_MODE_CHASE_FUGITIVE) {
            ctrl_or_grid_cars = 4;
          }
          else {
            event_type = get_event_type();
            if (event_type == EVENT_TRAINING) {
              event_type = training_lesson_get();
              if (event_type == 0) {
                ctrl_or_grid_cars = ui_get_car_8c86c8();
                g_race_652f5c = (-(uint)(ctrl_or_grid_cars != 1) & 0xffffffed) + 0x16;
                order1_or_slot = order0_or_player + 4;
                ctrl_or_grid_cars = 4;
              }
              event_type = training_lesson_get();
              if (event_type == 1) {
                g_race_652f5c = 0x2b;
                order1_or_slot = 6;
                ctrl_or_grid_cars = 4;
              }
              event_type = training_lesson_get();
              if (event_type == 4) {
                g_race_652f5c = 0x45;
                order1_or_slot = 0;
                ctrl_or_grid_cars = 1;
              }
              event_type = training_lesson_get();
              if (event_type == 5) {
                g_race_652f5c = 0;
                order1_or_slot = 3 - order0_or_player;
                ctrl_or_grid_cars = 4;
              }
            }
          }
          grid_position(g_race_652f5c,order1_or_slot,ctrl_or_grid_cars,&frame.spawn_x,&frame.heading);
          player_set_precise_position(order0_or_player,(fbits1 = (float)(int)g_race_652f5c, *(int *)&fbits1));
          car_spawn(order0_or_player,frame.spawn_x,frame.spawn_y,frame.spawn_z,frame.heading,(undefined4)&g_car_801b0c);
          car_current_matrix(frame.car_matrix_ptr,order0_or_player);
          team_color_rgba(order0_or_player,frame.team_rgba);
          player_status_set(order0_or_player,1);
          idx_or_team_mode = idx_or_team_mode + 1;
        }
        order0_or_player = order0_or_player + 1;
      } while ((int)order0_or_player < 4);
      return;
    }
  } while( true );
}

