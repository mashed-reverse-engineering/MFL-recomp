// cluster-entries: player_stats_update
/* Installed cluster player_stats_update (tools/cluster_install.py from output/cluster_pilot_j1/player_stats_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern struct EventLogArgs g_event_log_args[256];
/*00408ea0*/ extern int player_record_get_8d8ca0();
/*0040fdc0*/ extern int race_get_phase();
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*0040ff20*/ extern int race_setup_get_slot_type();
/*004183b0*/ extern undefined __cdecl ai_bot_set_timer_30000(undefined4 p0);
/*00426f50*/ extern float atan2_degrees(float param_2, float param_3);
/*0042b1d0*/ extern int course_get_record_base();
/*0047a830*/ extern int car_is_active();
/*0047ac30*/ extern int car_get_mode(int p0, int *p1, int *p2);
/*0047ac90*/ extern int car_get_int_from_float_field();
/*0047b590*/ extern int car_get_forward();
/*004b3284*/ extern double FUN_004b3284(double p0);
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005dd7e8*/ extern float k_f_60;
/*005ddacc*/ extern float k_f_360;
/*005ddad0*/ extern float k_f_180;
/*005ddad4*/ extern float k_f_90;
/*005e64a4*/ extern float k_f_0;
/*005de21c*/ extern float k_f_57_2958;
/*005de250*/ extern float k_f_330;
/*005de594*/ extern float k_f_350;
/*007e7f60*/ extern undefined4 g_event_log_head;
/*007e7f64*/ extern undefined4 g_event_log_type;
/*007e8364*/ extern undefined4 g_event_log_time;
/*007e8764*/ extern undefined4 g_event_log_param;
/*007ef7e0*/ extern undefined4 g_race_clock_b;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c8ae0*/ extern struct PlayerStatRecord g_player_stats_race[4];
/* player_stats_update */
/* event log record types written by this function (the header comment above names the group 0x3FF01..0x3FF04; the split is inferred from the code that writes each) */
#define EVLOG_STAT_KILL (0x3ff01)             /* a car died while the last hitter (stats.last_hit_by) hit it within 4.5 s: kill for the hitter */
#define EVLOG_STAT_UNFORCED_ERROR (0x3ff02)   /* a car died without a recent hitter */
#define EVLOG_STAT_AGGRESSION (0x3ff03)       /* a human car rammed by this car: aggression counter */
#define EVLOG_STAT_AGGRESSION_ANGLED (0x3ff04) /* the same, headings differ by more than 60 degrees */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(dt) per frame: turbo skills (steering spin, long reverse), aggression (ram of a human car),
   kills / unforced errors when a car dies, writes events 0x3FF01..0x3FF04 to the event log" */

void __cdecl player_stats_update(int dt_ticks,undefined4 unused_2,undefined4 unused_3,undefined4 unused_4,
                   undefined4 unused_5,undefined4 unused_6)

{
  double dot_or_part;
  double dot_clamped;
  int tmp_ok;
  undefined4 tmp_u;
  float *course_rec;
  int *other_stats_cursor;
  int *stats_cursor;
  int tmp_i;
  uint car_id_ff00;
  float10 angle_deg;
  struct {
    int car_index;
    int *other_clock_ptr;
    PlayerSlot *slot_ptr;
    int car_mode;
    float fwd_x;
    float fwd_y;
    float fwd_z;
    float other_fwd_x;
    float other_fwd_y;
    float other_fwd_z;
    int unused_words [9];
  } frame;
  
  int mode_a[4], mode_b[4];
  float partial;
  tmp_i = 0;
  do {
    car_get_mode(tmp_i,&mode_a[tmp_i],&mode_b[tmp_i]);
    tmp_i = tmp_i + 1;
  } while (tmp_i < 4);
  tmp_i = race_get_phase();
  if (tmp_i != 7) {
    /* per human-controlled car: steering spin (turbo skill), long reverse while braking, ramming of other cars (aggression) */
    frame.car_index = 0;
    frame.slot_ptr = g_player_slots;
    stats_cursor = &g_player_stats_race[0].turn_state;
    do {
      tmp_i = frame.slot_ptr->controller;
      tmp_ok = race_setup_slot_used(frame.car_index);
      if ((tmp_ok != 0) && (tmp_ok = car_is_active(frame.car_index), tmp_ok == 1)) {
        car_get_int_from_float_field(&frame.car_mode, frame.car_index);
        if (frame.car_mode == 4) {
          tmp_u = car_get_forward(&frame.fwd_x, frame.car_index);
          angle_deg = (float10)atan2_degrees(frame.fwd_x,frame.fwd_z);
          if (g_player_inputs[tmp_i].left == 0) {
            if (g_player_inputs[tmp_i].right == 0) {
              *stats_cursor = 0;
            }
            else {
              if (*stats_cursor == 2) {
                angle_deg = (float10)*(float *)&stats_cursor[-1] - angle_deg;
                if (angle_deg < k_f_0) {
                  do {
                    angle_deg = angle_deg + (float10)k_f_360;
                  } while (angle_deg < k_f_0);
                }
                goto LAB_00428f73;
              }
              *(float *)&stats_cursor[-1] = (float)angle_deg;
              *stats_cursor = 2;
            }
          }
          else if (*stats_cursor == 1) {
            angle_deg = angle_deg - (float10)*(float *)&stats_cursor[-1];
            if (angle_deg < k_f_0) {
              do {
                angle_deg = angle_deg + (float10)k_f_360;
              } while (angle_deg < k_f_0);
            }
LAB_00428f73:
            if (((float10)k_f_330 < angle_deg) && (angle_deg < (float10)k_f_350)) {
              stats_cursor[-0x3f] = stats_cursor[-0x3f] + 200;
              *stats_cursor = 0;
              stats_cursor[-0x3e] = 1;
              stats_cursor[-0x3d] = 3000;
            }
          }
          else {
            *(float *)&stats_cursor[-1] = (float)angle_deg;
            *stats_cursor = 1;
          }
        }
        if (g_player_inputs[tmp_i].brake != 0) {
          car_get_forward(&frame.fwd_x, frame.car_index);
          tmp_u = player_record_get_8d8ca0(frame.car_index);
          course_rec = (float *)course_get_record_base(tmp_u);
          tmp_u = player_record_get_8d8ca0(frame.car_index);
          tmp_i = course_get_record_base(tmp_u);
          dot_or_part = *(float *)(tmp_i + 4) * frame.fwd_y;
          dot_clamped = frame.fwd_x * *course_rec;
          tmp_u = player_record_get_8d8ca0(frame.car_index);
          tmp_i = course_get_record_base(tmp_u);
          partial = (float)(dot_clamped + dot_or_part);
          if (*(float *)(tmp_i + 8) * frame.fwd_z + partial < k_f_0) {
            tmp_i = stats_cursor[1];
            stats_cursor[1] = tmp_i + dt_ticks;
            if (tmp_i + dt_ticks < 0x2329) goto LAB_00429069;
            stats_cursor[-0x3f] = stats_cursor[-0x3f] + 200;
            stats_cursor[-0x3e] = 2;
            stats_cursor[-0x3d] = 3000;
          }
          stats_cursor[1] = 0;
        }
LAB_00429069:
        frame.other_clock_ptr = stats_cursor + 2;
        tmp_i = 0;
        other_stats_cursor = &g_player_stats_race[0].aggression;
        do {
          if (((((frame.car_index != tmp_i) && (tmp_ok = race_setup_slot_used(tmp_i), tmp_ok != 0))
               && (tmp_ok = car_is_active(tmp_i), tmp_ok == 1)) &&
              ((*frame.other_clock_ptr == g_race_clock_b && (stats_cursor[-0x1a] = stats_cursor[-0x1a] + 1, frame.car_mode == 4))))
             && (stats_cursor[0xc] == 1)) {
            *other_stats_cursor = *other_stats_cursor + 1;
            tmp_ok = race_get_phase();
            tmp_u = g_last_frame_ticks;
            if (((tmp_ok != 1) && (tmp_ok != 2)) && ((tmp_ok != 3 && ((tmp_ok != 4 && (tmp_ok != 5))))))
            {
              (&g_event_log_type)[g_event_log_head] = EVLOG_STAT_AGGRESSION;
              (&g_event_log_time)[g_event_log_head] = tmp_u;
              (&g_event_log_param)[g_event_log_head] = tmp_i + 0xff00;
              g_event_log_args[g_event_log_head].arg0 = frame.car_index + 0xff00;
              g_event_log_head = g_event_log_head + 1;
              if (0xff < g_event_log_head) {
                g_event_log_head = 0;
              }
            }
            tmp_ok = race_setup_get_slot_type(tmp_i);
            if (tmp_ok == 1) {
              ai_bot_set_timer_30000(frame.car_index);
            }
            car_get_forward(&frame.fwd_x,frame.car_index);
            car_get_forward(&frame.other_fwd_x,tmp_i);
            dot_or_part = frame.other_fwd_z * frame.fwd_z + frame.other_fwd_x * frame.fwd_x + frame.other_fwd_y * frame.fwd_y;
            dot_clamped = k_f_n1;
            if ((k_f_n1 <= dot_or_part) && (dot_clamped = dot_or_part, float_1_0 < dot_or_part)) {
              dot_clamped = float_1_0;
            }
            angle_deg = (float10)FUN_004b3284((double)dot_clamped);
            angle_deg = angle_deg * (float10)k_f_57_2958;
            if (angle_deg < k_f_0) {
              angle_deg = -angle_deg;
            }
            if ((float10)k_f_180 < angle_deg) {
              do {
                angle_deg = angle_deg - (float10)k_f_180;
              } while ((float10)k_f_180 < angle_deg);
            }
            if ((float10)k_f_90 < angle_deg) {
              angle_deg = (float10)k_f_180 - angle_deg;
            }
            if (((((float10)k_f_60 < angle_deg) &&
                 (tmp_ok = race_get_phase(), tmp_u = g_last_frame_ticks, tmp_ok != 1)) && (tmp_ok != 2)
                ) && (((tmp_ok != 3 && (tmp_ok != 4)) && (tmp_ok != 5)))) {
              (&g_event_log_type)[g_event_log_head] = EVLOG_STAT_AGGRESSION_ANGLED;
              (&g_event_log_time)[g_event_log_head] = tmp_u;
              (&g_event_log_param)[g_event_log_head] = tmp_i + 0xff00;
              g_event_log_args[g_event_log_head].arg0 = frame.car_index + 0xff00;
              g_event_log_head = g_event_log_head + 1;
              if (0xff < g_event_log_head) {
                g_event_log_head = 0;
              }
            }
          }
          frame.other_clock_ptr = frame.other_clock_ptr + 1;
          other_stats_cursor = other_stats_cursor + 0x4e;
          tmp_i = tmp_i + 1;
        } while ((int)other_stats_cursor < (int)&g_player_stats_race + 0x580);
      }
      frame.car_index = frame.car_index + 1;
      stats_cursor = stats_cursor + 0x4e;
      frame.slot_ptr = frame.slot_ptr + 1;
    } while ((int)stats_cursor < (int)&g_player_stats_race + 0x5e4);
  }
  /* second pass: a car that is no longer active produces a death event (5) and a kill / unforced error statistic */
  stats_cursor = mode_a;
  other_stats_cursor = &g_player_stats_race[0].last_hit_by;
  car_id_ff00 = 0xff00;
  do {
    if (((other_stats_cursor[-0x20] == 0) &&
        (tmp_i = race_setup_slot_used(car_id_ff00 - 0xff00), tmp_i != 0)) &&
       ((*stats_cursor != 0 || (tmp_i = car_is_active(car_id_ff00 - 0xff00), tmp_i != 1)))) {
      tmp_i = race_get_phase();
      tmp_u = g_last_frame_ticks;
      if ((((tmp_i != 1) && (tmp_i != 2)) && (tmp_i != 3)) &&
         ((tmp_i != 4 && (tmp_i != 5)))) {
        (&g_event_log_type)[g_event_log_head] = 5;
        (&g_event_log_time)[g_event_log_head] = tmp_u;
        (&g_event_log_param)[g_event_log_head] = car_id_ff00;
        g_event_log_head = g_event_log_head + 1;
        if (0xff < g_event_log_head) {
          g_event_log_head = 0;
        }
      }
      if (*other_stats_cursor < 1000) {
        other_stats_cursor[-0x21] = other_stats_cursor[-0x21] + 1;
        tmp_i = race_get_phase();
        tmp_u = g_last_frame_ticks;
        if (((tmp_i != 1) && (tmp_i != 2)) &&
           ((tmp_i != 3 && ((tmp_i != 4 && (tmp_i != 5)))))) {
          (&g_event_log_type)[g_event_log_head] = EVLOG_STAT_UNFORCED_ERROR;
          (&g_event_log_time)[g_event_log_head] = tmp_u;
          (&g_event_log_param)[g_event_log_head] = car_id_ff00;
LAB_0042955f:
          g_event_log_head = g_event_log_head + 1;
          if (0xff < g_event_log_head) {
            g_event_log_head = 0;
          }
        }
      }
      else if ((int)g_race_clock_b - other_stats_cursor[1] < 0x1194) {
        tmp_i = race_get_phase();
        tmp_u = g_last_frame_ticks;
        if (((tmp_i != 1) && (tmp_i != 2)) &&
           ((tmp_i != 3 && ((tmp_i != 4 && (tmp_i != 5)))))) {
          (&g_event_log_type)[g_event_log_head] = EVLOG_STAT_KILL;
          tmp_i = *other_stats_cursor;
          (&g_event_log_time)[g_event_log_head] = tmp_u;
          (&g_event_log_param)[g_event_log_head] = tmp_i + 0xfb18;
          g_event_log_args[g_event_log_head].arg0 = car_id_ff00;
          g_event_log_head = g_event_log_head + 1;
          if (0xff < g_event_log_head) {
            g_event_log_head = 0;
          }
        }
        g_player_stats_race[*other_stats_cursor + -1000].kills = g_player_stats_race[*other_stats_cursor + -1000].kills + 1;
        *other_stats_cursor = 0;
      }
      else {
        other_stats_cursor[-0x21] = other_stats_cursor[-0x21] + 1;
        tmp_i = race_get_phase();
        tmp_u = g_last_frame_ticks;
        if ((((tmp_i != 1) && (tmp_i != 2)) && (tmp_i != 3)) &&
           ((tmp_i != 4 && (tmp_i != 5)))) {
          (&g_event_log_type)[g_event_log_head] = EVLOG_STAT_UNFORCED_ERROR;
          (&g_event_log_time)[g_event_log_head] = tmp_u;
          (&g_event_log_param)[g_event_log_head] = car_id_ff00;
          goto LAB_0042955f;
        }
      }
      other_stats_cursor[-0x20] = 1;
    }
    other_stats_cursor = other_stats_cursor + 0x4e;
    stats_cursor = stats_cursor + 1;
    tmp_i = car_id_ff00 - 0xfeff;
    car_id_ff00 = car_id_ff00 + 1;
    if (3 < tmp_i) {
      return;
    }
  } while( true );
}

