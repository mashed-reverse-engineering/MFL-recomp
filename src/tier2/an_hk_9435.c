#include "mfl_tier2.h"
extern struct EventLogArgs g_event_log_args[256];
extern struct EventLogData g_event_log_data[256];
/*0040ff20*/ extern int race_setup_get_slot_type(int slot);
/*00419f70*/ extern void __cdecl ai_bot_set_cooldown_b4(int car);
/*00424b20*/ extern void __cdecl debris_slot_release_by_clump(unsigned int clump);
/*00434bb0*/ extern int get_event_type(void);
/*00437f10*/ extern int training_lesson_get(void);
/*00437f20*/ extern int ui_get_car_8c86c8(void);
/*004591b0*/ extern void course_highway_hit_spawn_fx(void *hit);
/*0047ac30*/ extern void car_get_mode(int car, int *mode, int *extra);
/*0047ac60*/ extern void car_set_mode(int car, int mode, int extra);
/*0048e470*/ extern void rwp_object_sync_and_reset(unsigned int object, struct RwV3d *offset);
/*00491ab0*/ extern void car_enter_rwp_mode(int car);
/*00492100*/ extern void rwp_car_apply_forces(int car, int mode);
/*004a1680*/ extern void player_rumble(int car, int kind, int duration, int strength);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *v);
/*005dd62c*/ extern float k_f_100;
/*005de220*/ extern float k_f_2500;
/*005de25c*/ extern float k_f_0_005;
/*007e7f60*/ extern int g_event_log_head;
/*007e7f64*/ extern int g_event_log_type[256];
/*007e8364*/ extern int g_event_log_time[256];
/*007e8764*/ extern unsigned int g_event_log_param[256];
/*007ef7e0*/ extern int g_race_clock_b;
/*007ef818*/ extern int g_last_frame_ticks;
/*008c8ae0*/ extern struct PlayerStatRecord g_player_stats_race[4];
/* record handed to course_highway_hit_spawn_fx (type 10): the event data words, then the car and two event arguments */
typedef struct HitFxRecord {
  unsigned int data[15];      /* g_event_log_data words 0x00..0x38 */
  unsigned int car;
  unsigned int arg0;
  unsigned int arg2;
} HitFxRecord;
/* race_events_process_log */
/* "walks the event log ring of this tick: type 3 wall hit (wall_hits++, rumble, rwp mode at high
   speed), type 4 car to car collision (last_hit_by, collision_speed), types 6 / 8 / 9 / 10 effects" */
int race_events_process_log(void)
{
  int event_type;
  int modes[4];
  int extra[4];
  int mode;
  int tmp;
  int idx;
  int remaining;
  int stop;
  int ticks;
  unsigned int car;
  unsigned int other;
  unsigned int colour;
  int type;
  int k;
  int clock;
  int other_arg;
  float len;
  RwV3d vec;
  HitFxRecord hit;
  PlayerStatRecord *stats_a;
  PlayerStatRecord *stats_b;

  event_type = get_event_type();
  for (k = 0; k < 4; k++) {
    car_get_mode(k, &modes[k], &extra[k]);
  }
  idx = g_event_log_head - 1;
  if (idx < 0) {
    idx = 0xff;
  }
  ticks = g_last_frame_ticks;
  remaining = 0x100;
  stop = 0;
  do {
    if (stop != 0) {
      return 1;
    }
    if (g_event_log_time[idx] == ticks) {
      car = g_event_log_param[idx] - 0xff00;
      type = g_event_log_type[idx];
      if (type == 10) {
        hit.arg0 = g_event_log_args[idx].arg0;
        hit.arg2 = g_event_log_args[idx].arg2;
        hit.car = car;
        for (k = 0; k < 12; k++) {
          hit.data[k] = ((unsigned int *)&g_event_log_data[idx])[k];
        }
        hit.data[12] = g_event_log_data[idx].w30;
        hit.data[13] = g_event_log_data[idx].w34;
        hit.data[14] = g_event_log_data[idx].w38;
        course_highway_hit_spawn_fx(&hit);
      }
      else if (type == 9) {
        debris_slot_release_by_clump(g_event_log_args[idx].arg1);
      }
      if (car < 0x10) {
        if (type == 8) {
          vec.y = g_event_log_data[idx].w04 * k_f_0_005;
          vec.x = 0.0f;
          vec.z = 0.0f;
          rwp_object_sync_and_reset(g_event_log_args[idx].arg1, &vec);
        }
        else if (type == 6) {
          colour = g_event_log_args[idx].arg0;
          car_get_mode(car, &mode, &tmp);
          if (colour == 0xff0000ff || colour == 0xffff0080) {
            if (mode == 0 || mode == 1) {
              car_set_mode(car, 2, 0);
              player_rumble(car, 2, 0x1e, 0xff);
            }
          }
          else if (colour == 0xff010101) {
            if (mode == 0) {
              car_set_mode(car, 1, 0);
            }
          }
        }
        else if (type == 4) {
          car = g_event_log_args[idx].arg0;
          other = g_event_log_args[idx].arg1;
          player_rumble(car, 1, 2, 0xff);
          player_rumble(other, 1, 2, 0xff);
          if (modes[car] == 0 && modes[other] == 0) {
            clock = g_race_clock_b;
            stats_a = &g_player_stats_race[car];
            stats_b = &g_player_stats_race[other];
            stats_a->last_hit_by = other + 1000;
            stats_a->last_hit_time = clock;
            stats_b->last_hit_time = clock;
            stats_b->last_hit_by = car + 1000;
            stats_a->last_collision_time[other] = clock;
            stats_b->last_collision_time[car] = clock;
            *(unsigned int *)&vec.x = g_event_log_data[idx].w24;
            *(unsigned int *)&vec.y = g_event_log_data[idx].w28;
            *(unsigned int *)&vec.z = g_event_log_data[idx].w2c;
            len = RwV3dLength(&vec);
            stats_a->collision_speed[other] = len;
            stats_b->collision_speed[car] = len;
            other_arg = g_event_log_args[idx].arg2;
            stats_a->other_is_human = 0;
            stats_b->other_is_human = (other_arg == 1);
            if (race_setup_get_slot_type(car) != 1) {
              ai_bot_set_cooldown_b4(car);
            }
            if (race_setup_get_slot_type(other) != 1) {
              ai_bot_set_cooldown_b4(other);
            }
          }
        }
        else if (type == 3) {
          if (event_type == 0x11 && training_lesson_get() == 1 && ui_get_car_8c86c8() != 1 && ui_get_car_8c86c8() != 4) {
            car_set_mode(0, 1, 0);
            player_rumble(0, 2, 0x1e, 0xff);
          }
          g_player_stats_race[car].wall_hits = g_player_stats_race[car].wall_hits + 1;
          *(unsigned int *)&vec.x = g_event_log_data[idx].w10;
          *(unsigned int *)&vec.y = g_event_log_data[idx].w14;
          *(unsigned int *)&vec.z = g_event_log_data[idx].w18;
          len = RwV3dLength(&vec);
          if (len > k_f_100) {
            player_rumble(car, 1, 2, 0xff);
          }
          if (len > k_f_2500) {
            car_enter_rwp_mode(car);
            rwp_car_apply_forces(car, 7);
          }
        }
      }
    }
    else {
      stop = 1;
    }
    idx = idx - 1;
    if (idx < 0) {
      idx = 0xff;
    }
    remaining = remaining - 1;
  } while (remaining > 0);
  return 1;
}
