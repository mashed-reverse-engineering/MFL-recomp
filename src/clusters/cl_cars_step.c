// cluster-entries: cars_step
/* Installed cluster cars_step (tools/cluster_install.py from output/cluster_pilot_md/cars_step.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00409eb0*/ extern double player_rank_key();
/*0040fdb0*/ extern int get_player_count();
/*0040fdc0*/ extern int race_get_phase();
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*0040ff20*/ extern int race_setup_get_slot_type();
/*004162f0*/ extern int trail_set_param();
/*00426b40*/ extern int car_kill();
/*00434bb0*/ extern int get_event_type();
/*00435400*/ extern int race_length_setting();
/*0044c540*/ extern int camera_get_car_8c747c();
/*004758e0*/ extern undefined cars_update_timers(int p1);
/*0047a830*/ extern int car_is_active();
/*0047bb00*/ extern int car_world_collision_query();
/*0047bca0*/ extern undefined __cdecl car_log_event_6(undefined4 p0);
/*0047f030*/ extern undefined __cdecl cars_update_all(undefined4 p0, undefined4 p1);
/*00490570*/ extern int rwp_cars_update();
/*00491ab0*/ extern int car_enter_rwp_mode();
/*00491d70*/ extern int car_reset_physics();
/*00491f10*/ extern undefined __cdecl car_apply_throttle_force(undefined4 p0);
/*004b2b48*/ extern int _ftol();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd33c*/ extern float k_f_4;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005e64a4*/ extern float k_f_0;
/*005dde3c*/ extern float k_f_3000;
/*005de1b4*/ extern float k_f_0_0625;
/*005df9d0*/ extern float k_f_15000;
/*005dfda0*/ extern float k_f_0_0027778;
/*005dfdb8*/ extern float k_f_12000;
/*007e7f60*/ extern undefined4 g_event_log_head;
/*007e7f64*/ extern undefined4 g_event_log_type;
/*007e8364*/ extern undefined4 g_event_log_time;
/*007e8764*/ extern undefined4 g_event_log_param;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7bc*/ extern undefined4 g_cars_7ef7bc;
/*007ef7c0*/ extern undefined4 g_cars_7ef7c0;
/*007ef800*/ extern undefined4 g_time_limit_ticks;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01a0*/ extern undefined1 g_cars_7f01a0;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*00801b1c*/ extern float g_gravity_vector;
/*00801b20*/ extern float g_cars_801b20;
/*00801b24*/ extern float g_cars_801b24;
/*00801b28*/ extern float g_cars_801b28;
/*008b06e0*/ extern struct Car player_car[16];
/*008bd730*/ extern float g_car_8bd730;
/*008bd750*/ extern float g_cars_8bd750;
/*008bd7a0*/ extern undefined4 g_rank_order;
/*008bd7a4*/ extern undefined4 g_cars_step_8bd7a4;
/*008bd7a8*/ extern undefined4 g_cars_8bd7a8;
/*008bd7ac*/ extern undefined4 g_cars_8bd7ac;
/*008d8b30*/ extern undefined4 player_count;
/*007ef7dc*/ extern int g_race_clock_a;
static int car_update_r(Car * reg_eax, int step_params, float param_3, byte * param_4, void *param_5);
extern void car_update(void);
__declspec(naked) int car_update_r(Car * reg_eax, int step_params, float param_3, byte * param_4, void *param_5)
{
    __asm {
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        mov eax, dword ptr [esp + 20]
        call car_update
        add esp, 16
        ret
    }
}

static int car_wheel_contact_update_r(void *reg_eax, int reg_edi, int stk0);
extern void car_wheel_contact_update(void);
__declspec(naked) int car_wheel_contact_update_r(void *reg_eax, int reg_edi, int stk0)
{
    __asm {
        push edi
        push dword ptr [esp + 16]
        mov eax, dword ptr [esp + 12]
        mov edi, dword ptr [esp + 16]
        call car_wheel_contact_update
        add esp, 4
        pop edi
        ret
    }
}

/* cars_step */

/* "physics driver per game tick: race time limit, ranking of the players, then chunks of <= 50
   ticks: car_update for every slot, speed lengths, sub-steps of <= 25 ticks of cars_update_all" */

undefined4 cars_step(int tick_count,float *step_params)

{
  float tmp_f;
  bool unused_bool_a;
  bool unused_bool_b;
  float step_f;
  float rank_key[4];
  int swapped, j;
  int ticks;      /* ticks still to simulate */
  unsigned int chunk;      /* ticks simulated in this pass (<= 50) */
  unsigned int rem, step;   /* sub-steps of cars_update_all (<= 25 ticks) */
  int car_index;
  float avg_z_sum;
  float *history_src;
  undefined4 frame_ticks;
  uint *car_cursor;
  uint player_count;
  undefined4 unused_u;
  float avg_x_sum;
  int *rwp_timer_cursor;
  uint rank_i;
  PlayerInput *input;
  int inner_i;
  float *history_dst;
  float10 fl_tmp;
  RwReal vec_len;
  struct {
    PlayerSlot *slot_ptr;
    undefined1 pad_1c[4];
    int cars_left;
    undefined1 pad_14[4];
  } frame;
  
  /* Hold-the-Flag: when the flag holder is dead, kill everybody; Hare-and-Hounds: every slot except the hare gets state 1 */
  if (game_mode == GAME_MODE_HOLD_THE_FLAG) {
    if (((int)g_cars_7ef7bc < 0) || (3 < (int)g_cars_7ef7bc)) goto LAB_0047f3f4;
    car_index = car_is_active(g_cars_7ef7bc);
    if (car_index == 0) {
      player_count = 0;
      do {
        car_index = race_setup_slot_used(player_count);
        if (car_index != 0) {
          car_kill(player_count);
        }
        player_count = player_count + 1;
      } while (player_count < 4);
    }
    car_index = race_get_phase();
    trail_set_param(g_cars_7ef7bc,3,car_index == 6);
  }
  if (game_mode == GAME_MODE_HARE_AND_HOUNDS) {
    if (((int)g_cars_7ef7c0 < 0) || (car_index = get_player_count(), car_index <= (int)g_cars_7ef7c0)) {
      g_cars_7ef7c0 = 0;
    }
    player_count = 0;
    do {
      *(undefined4 *)((int)&g_player_slots[0].state + player_count) = 1;
      player_count = player_count + 0x10;
    } while (player_count < 0x40);
    g_player_slots[g_cars_7ef7c0].state = 0;
    car_index = race_get_phase();
    trail_set_param(g_cars_7ef7c0,3,car_index == 6);
  }
LAB_0047f3f4:
  /* race time limit by race length setting (15 / 30 / 60 min), 100 h in training; the 2 special modes use race clock + 30 s */
  g_time_limit_ticks = 900000;
  car_index = race_length_setting();
  if (car_index == 1) {
    g_time_limit_ticks = 1800000;
  }
  else if (car_index == 2) {
    g_time_limit_ticks = 3600000;
  }
  car_index = get_event_type();
  if (car_index == 0x11) {
    g_time_limit_ticks = 360000000;
  }
  if ((game_mode == GAME_MODE_HOLD_THE_FLAG) || (car_index = game_mode, game_mode == GAME_MODE_HARE_AND_HOUNDS)) {
    car_index = ((int)((float)g_race_clock_a + 30000.0f));
    g_time_limit_ticks = car_index;
  }
  cars_update_timers(tick_count);
  rwp_cars_update();
  /* ranking: bubble sort of the players by rank key */
  rank_key[0] = 1000.0;
  rank_key[1] = 1000.0;
  rank_key[2] = 1000.0;
  player_count = 0;
  rank_key[3] = 1000.0;
  do {
    (&g_rank_order)[player_count] = player_count;
    player_count = player_count + 1;
  } while (player_count < 4);
  player_count = get_player_count();
  for (rank_i = 0; rank_i < player_count; rank_i = rank_i + 1) {
    rank_key[rank_i] = (float)player_rank_key(rank_i);
  }
  /* bubble sort of the players by rank key, best (largest) first, g_rank_order follows the keys */
  do {
    swapped = 0;
    for (j = 0; j < 3; j = j + 1) {
      if (rank_key[j] < rank_key[j + 1]) {
        float key_tmp = rank_key[j];
        int order_tmp = (&g_rank_order)[j];
        rank_key[j] = rank_key[j + 1];
        rank_key[j + 1] = key_tmp;
        (&g_rank_order)[j] = (&g_rank_order)[j + 1];
        (&g_rank_order)[j + 1] = order_tmp;
        swapped = 1;
      }
    }
  } while (swapped);
  /* per tick constants from the step length */
  g_cars_8bd750 = *step_params * k_f_0_0027778;
  g_car_8bd730 = k_f_3000 / g_cars_8bd750;
  /* main loop: chunks of <= 50 ticks; the pass with no ticks left finalises and returns 1 */
  ticks = tick_count;
  do {
    if (ticks == 0) {
      car_index = 0;
      /* finalise 1: per car, velocity average of the last 16 samples, idle timer / log event 1 when stuck > 15 s */
      car_cursor = &player_car[0].matrix_current;
      frame.cars_left = 0x10; /* MAX_CARS */
      do {
        if ((car_cursor[-0x265] == 0) && ((car_cursor[-0x269] != 0 || (car_cursor[-0x266] == 1)))) {
          player_count = *car_cursor;
          if (player_count != 0) {
            car_cursor[1] = player_count;
            *car_cursor = player_count - 1 & 1;
            history_src = (float *)(car_cursor + -0x20);
            history_dst = (float *)(car_cursor + -0x10);
            for (inner_i = 0x10; inner_i != 0; inner_i = inner_i + -1) {
              *history_dst = *history_src;
              history_src = history_src + 1;
              history_dst = history_dst + 1;
            }
          }
          inner_i = 2;
          history_src = (float *)(car_cursor + 100);
          avg_z_sum = k_f_0;
          tmp_f = k_f_0;
          avg_x_sum = k_f_0;
          do {
            inner_i = inner_i + -1;
            avg_x_sum = avg_x_sum + history_src[-3];
            avg_x_sum = avg_x_sum + history_src[0];
            avg_x_sum = avg_x_sum + history_src[3];
            avg_x_sum = avg_x_sum + history_src[6];
            avg_x_sum = avg_x_sum + history_src[9];
            avg_x_sum = avg_x_sum + history_src[0xc];
            avg_x_sum = avg_x_sum + history_src[0xf];
            avg_x_sum = avg_x_sum + history_src[0x12];
            tmp_f = tmp_f + history_src[-2];
            tmp_f = tmp_f + history_src[1];
            tmp_f = tmp_f + history_src[4];
            tmp_f = tmp_f + history_src[7];
            tmp_f = tmp_f + history_src[10];
            tmp_f = tmp_f + history_src[0xd];
            tmp_f = tmp_f + history_src[0x10];
            tmp_f = tmp_f + history_src[0x13];
            avg_z_sum = avg_z_sum + history_src[-1];
            avg_z_sum = avg_z_sum + history_src[2];
            avg_z_sum = avg_z_sum + history_src[5];
            avg_z_sum = avg_z_sum + history_src[8];
            avg_z_sum = avg_z_sum + history_src[0xb];
            avg_z_sum = avg_z_sum + history_src[0xe];
            avg_z_sum = avg_z_sum + history_src[0x11];
            avg_z_sum = avg_z_sum + history_src[0x14];
            history_src = history_src + 0x18;
          } while (inner_i != 0);
          *(float *)&car_cursor[0x53] = (avg_x_sum * k_f_0_0625);
          *(float *)&car_cursor[0x54] = (tmp_f * k_f_0_0625);
          *(float *)&car_cursor[0x55] = (avg_z_sum * k_f_0_0625);
          if (*(float *)&car_cursor[0xe] == k_f_0) {
            avg_z_sum = (float)(int)tick_count;
            if ((int)tick_count < 0) {
              avg_z_sum = avg_z_sum + k_f_4_2949673e9;
            }
            tmp_f = *(float *)&car_cursor[0x52];
            *(float *)&car_cursor[0x52] = (avg_z_sum + tmp_f);
            if (((((k_f_15000 < avg_z_sum + tmp_f) &&
                  (inner_i = race_get_phase(), frame_ticks = g_last_frame_ticks, inner_i != 1)) &&
                 (inner_i != 2)) && ((inner_i != 3 && (inner_i != 4)))) && (inner_i != 5)) {
              (&g_event_log_type)[g_event_log_head] = 1;
              (&g_event_log_time)[g_event_log_head] = frame_ticks;
              (&g_event_log_param)[g_event_log_head] = car_index + 0xff00;
              g_event_log_head = g_event_log_head + 1;
              if (0xff < (int)g_event_log_head) {
                g_event_log_head = 0;
              }
            }
          }
          else {
            car_cursor[0x52] = 0;
          }
        }
        car_index = car_index + 1;
        car_cursor = car_cursor + 0x341;
        frame.cars_left = frame.cars_left + -1;
      } while (frame.cars_left != 0);
      car_index = 0;
      /* finalise 2: per car: rwp timer countdown, world collision query, wheel contact, throttle force, rwp mode switch / physics reset */
      rwp_timer_cursor = &player_car[0].rwp_timer;
      frame.cars_left = 0x10; /* MAX_CARS */
      do {
        if ((rwp_timer_cursor[-5] != 0) || (rwp_timer_cursor[-2] == 1)) {
          if ((*rwp_timer_cursor != 0) && (inner_i = *rwp_timer_cursor - (int)tick_count, *rwp_timer_cursor = inner_i, inner_i < 0))
          {
            *rwp_timer_cursor = 0;
          }
          if (rwp_timer_cursor[-1] != 0) {
            inner_i = car_world_collision_query(car_index,step_params);
            *(float *)&rwp_timer_cursor[0x272] = (float)inner_i;
            if ((float)inner_i == k_f_4) {
              car_wheel_contact_update_r((void *)step_params,(int)((Car *)(rwp_timer_cursor + -6)),0);
            }
            car_apply_throttle_force(car_index);
          }
          if (*rwp_timer_cursor == 0) {
            if (*(float *)&rwp_timer_cursor[0x272] < k_f_4) {
              car_enter_rwp_mode(car_index);
            }
            if ((rwp_timer_cursor[0x272] == 0x40800000 /* 4.0f, compared as an int word */) && (rwp_timer_cursor[0x276] == 0)) {
              car_reset_physics(car_index);
            }
          }
        }
        car_index = car_index + 1;
        rwp_timer_cursor = rwp_timer_cursor + 0x341;
        frame.cars_left = frame.cars_left + -1;
      } while (frame.cars_left != 0);
      g_cars_801b24 = 0;
      g_cars_801b20 = 0;
      g_gravity_vector = 0;
      g_cars_801b28 = 0;
      return 1;
    }
    /* per chunk: car_update for every slot (input from its controller, zero input unless racing) */
    chunk = 50;
    if ((uint)ticks < 0x32) {
      chunk = ticks;
    }
    player_count = 0;
    frame.slot_ptr = g_player_slots;
    car_cursor = &player_car[0].mode;
    frame.cars_left = 0x10; /* MAX_CARS */
    do {
      ((Car *)(car_cursor + -0x27c))->index = player_count;
      rank_i = 0;
      car_index = race_setup_get_slot_type(player_count);
      if (((car_index != 0) && (car_index != 1)) || (car_index = camera_get_car_8c747c(), car_index == 1)) {
        rank_i = 1;
      }
      car_cursor[0xc4] = rank_i;
      input = g_player_inputs + frame.slot_ptr->controller;
      if (*car_cursor != 0) {
        input = (PlayerInput *)&g_cars_7f01a0;
      }
      if (car_cursor[-0x278] != 0) {
        input = (PlayerInput *)&g_cars_7f01a0;
      }
      car_index = race_get_phase();
      if (((car_index != 6) && (car_index = race_get_phase(), car_index != 0xb)) &&
         (car_index = race_get_phase(), car_index != RACE_PHASE_WINNER_DONE)) {
        input = (PlayerInput *)&g_cars_7f01a0;
      }
      if ((car_cursor[-0x277] == 0) && ((car_cursor[-0x27b] != 0 || (car_cursor[-0x278] == 1)))) {
        car_cursor[-0x279] = 0;
        if (car_cursor[0x3f] != 0) {
          car_index = car_cursor[0x3f] - (int)chunk;
          car_cursor[0x3f] = car_index;
          if (car_index < -0x4b0) {
            car_cursor[0x3f] = 0;
          }
          if (-1 < (int)car_cursor[0x3f]) goto LAB_0047f6ce;
        }
        tmp_f = (float)(int)chunk;
        if ((int)chunk < 0) {
          tmp_f = tmp_f + k_f_4_2949673e9;
        }
        car_update_r((Car *)(car_cursor + -0x27c),player_count,tmp_f,(byte *)&input->left,(void *)step_params);
      }
LAB_0047f6ce:
      frame.slot_ptr = frame.slot_ptr + 1;
      player_count = player_count + 1;
      car_cursor = car_cursor + 0x341;
      frame.cars_left = frame.cars_left + -1;
    } while (frame.cars_left != 0);
    inner_i = 0;
    car_cursor = &player_car[0].controls_disabled;
    car_index = 0x10; /* MAX_CARS */
    do {
      if ((car_cursor[-3] != 0) || (*car_cursor == 1)) {
        vec_len = RwV3dLength((RwV3d *)(car_cursor + 0x268));
        *(float *)&car_cursor[0x275] = vec_len;
        vec_len = RwV3dLength((RwV3d *)(car_cursor + 0x26b));
        *(float *)&car_cursor[0x276] = vec_len;
      }
      if (k_f_12000 < (float)(int)car_cursor[-1]) {
        car_log_event_6(inner_i);
      }
      inner_i = inner_i + 1;
      car_cursor = car_cursor + 0x341;
      car_index = car_index + -1;
    } while (car_index != 0);
    /* sub-steps of at most 25 ticks for cars_update_all */
    for (rem = chunk; rem != 0; rem = rem - step) {
      step = rem;
      if (0x19 < rem) {
        step = 25;
      }
      step_f = (float)(int)step;
      if ((int)step < 0) {
        step_f = step_f + k_f_4_2949673e9;
      }
      cars_update_all(*(int *)&step_f,(undefined4)step_params);
    }
    ticks = ticks - (int)chunk;
  } while( true );
}

