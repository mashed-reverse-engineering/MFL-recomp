#include "mfl_tier2.h"
extern struct EventLogArgs g_event_log_args[256];
extern struct EventLogData g_event_log_data[256];
/*0040fdc0*/ extern int race_get_phase();
/*00477130*/ extern int __cdecl car_wheel_contact_find_index(void *p0, void *p1);
/*005dd370*/ extern float k_f_0_3;
/*005dd628*/ extern float k_f_0_25;
/*005e64a4*/ extern float k_f_0;
/*005de47c*/ extern float k_f_n0_25;
/*007e7f60*/ extern undefined4 g_event_log_head;
/*007e7f64*/ extern undefined4 g_event_log_type;
/*007e8364*/ extern undefined4 g_event_log_time;
/*007e8764*/ extern undefined4 g_event_log_param;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
/*008bd720*/ extern undefined4 g_car_8bd720[0x1000];
/*008bd74c*/ extern undefined4 g_collision_triangle_count;
/*008bd764*/ extern undefined4 g_car_8bd764;
/*008bd790*/ extern undefined4 g_car_8bd790;
#define TRI_MARK_FF010101 (0xff010101) /* triangle mark word [13] values that are logged as events; meaning of the colours unknown */
#define TRI_MARK_FF0000FF (0xff0000ff)
#define TRI_MARK_FFFF0080 (0xffff0080)
#define EVENT_DATA_WORDS (0x10) /* words copied from the stack snapshot into g_event_log_data (the snapshot struct below is uninitialised stack in the decompile, inferred) */
/* car_wheels_vs_triangles */

/* "wheel points against the collected triangle list in 0x8BD720 (inferred)" */

void car_wheels_vs_triangles(int *car,float *tri)

{
  float v0x;
  float v0y;
  float v0z;
  float v1x;
  float v1y;
  float v1z;
  float v2x;
  float v2y;
  float v2z;
  float nx;
  float ny;
  float nz;
  float px_or_dist;
  float py;
  uint tri_mark;
  float pz;
  undefined4 now_ticks;
  int phase_or_idx;
  int next_i;
  float *wheel_rec;
  float *point_ptr;
  int *point_done;
  uint tri_index;
  struct {
    float stack_w0;
    uint stack_w1;
    uint stack_w2;
    uint stack_w3;
    uint stack_w4;
    uint stack_w5;
    uint stack_w6;
    uint stack_w7;
  } frame;
  
  /* tri = 36-float collision triangle (v0 v1 v2 at [0..8], normal [9..11], mark [13], edge planes [0x1b..0x23]); car wheel points are tested one by one, the first 4 hits stop the scan */
  tri_index = 0;
  if (g_collision_triangle_count != 0) {
    do {
      v0x = *tri;
      v0y = tri[1];
      v0z = tri[2];
      v1x = tri[3];
      v1y = tri[4];
      v1z = tri[5];
      v2x = tri[6];
      v2y = tri[7];
      v2z = tri[8];
      nx = tri[9];
      ny = tri[10];
      nz = tri[0xb];
      wheel_rec = (float *)(car + 0x65);
      point_done = g_car_8bd720;
      point_ptr = (float *)&g_car_8bd764;
      do {
        if (*point_done == 0) {
          px_or_dist = point_ptr[-1];
          py = *point_ptr;
          pz = point_ptr[1];
          if ((k_f_0 <= (pz - v0z) * tri[0x1d] +
                        (py - v0y) * tri[0x1c] + (px_or_dist - v0x) * tri[0x1b]) &&
             (k_f_0 <= (pz - v1z) * tri[0x20] +
                       (py - v1y) * tri[0x1f] + (px_or_dist - v1x) * tri[0x1e])) {
            if ((k_f_0 <= (pz - v2z) * tri[0x23] +
                          (py - v2y) * tri[0x22] + (px_or_dist - v2x) * tri[0x21]) &&
               (!((px_or_dist = (pz - v2z) * nz +
                           (py - v2y) * ny + (px_or_dist - v2x) * nx) < k_f_n0_25) &&
                 (px_or_dist < k_f_0_25) &&
                (k_f_0_3 < nz * *(float *)&car[0x274] +
                           ny * *(float *)&car[0x273] + nx * *(float *)&car[0x272]))) {
              tri_mark = *(uint *)&tri[0xd];
              if (tri_mark == TRI_MARK_FF010101) {
                if (((((car[0x27c] == 0) &&
                      (phase_or_idx = race_get_phase(), now_ticks = g_last_frame_ticks, phase_or_idx != 1))
                     && (phase_or_idx != 2)) && ((phase_or_idx != 3 && (phase_or_idx != 4)))) &&
                   (phase_or_idx != 5)) {
                  (&g_event_log_type)[g_event_log_head] = 6;
                  (&g_event_log_time)[g_event_log_head] = now_ticks;
                  (&g_event_log_param)[g_event_log_head] = *car + 0xff00;
                  g_event_log_args[g_event_log_head].arg0 = TRI_MARK_FF010101;
                  g_event_log_args[g_event_log_head].arg1 = frame.stack_w1;
                  g_event_log_args[g_event_log_head].arg2 = frame.stack_w2;
                  g_event_log_args[g_event_log_head].w0c = frame.stack_w3;
                  g_event_log_args[g_event_log_head].w10 = frame.stack_w4;
                  g_event_log_args[g_event_log_head].w14 = frame.stack_w5;
                  g_event_log_args[g_event_log_head].w18 = frame.stack_w6;
                  g_event_log_args[g_event_log_head].w1c = frame.stack_w7;
                  phase_or_idx = 0;
                  do {
                    next_i = phase_or_idx + 1;
                    (&g_event_log_data[g_event_log_head].w00)[phase_or_idx] = (&frame.stack_w0)[phase_or_idx]
                    ;
                    phase_or_idx = next_i;
                  } while (next_i < EVENT_DATA_WORDS);
                  g_event_log_head = g_event_log_head + 1;
                  if (0xff < g_event_log_head) {
                    g_event_log_head = 0;
                  }
                }
              }
              else {
                phase_or_idx = car_wheel_contact_find_index(tri,car);
                if (phase_or_idx == 0) {
                  wheel_rec[0x1b] = tri[9];
                  wheel_rec[0x1c] = tri[10];
                  wheel_rec[0x1d] = tri[0xb];
                  *wheel_rec = px_or_dist;
                  wheel_rec[0x16] = tri[0xc];
                  wheel_rec[0x17] = tri[0xd];
                  if (tri_mark == TRI_MARK_FF0000FF) {
                    if (((((car[0x27c] == 0) || (car[0x27c] == 1)) &&
                         ((phase_or_idx = race_get_phase(), now_ticks = g_last_frame_ticks,
                          phase_or_idx != 1 && ((phase_or_idx != 2 && (phase_or_idx != 3)))))) &&
                        (phase_or_idx != 4)) && (phase_or_idx != 5)) {
                      (&g_event_log_type)[g_event_log_head] = 6;
                      (&g_event_log_time)[g_event_log_head] = now_ticks;
                      (&g_event_log_param)[g_event_log_head] = *car + 0xff00;
                      g_event_log_args[g_event_log_head].arg0 = TRI_MARK_FF0000FF;
                      g_event_log_args[g_event_log_head].arg1 = frame.stack_w1;
                      g_event_log_args[g_event_log_head].arg2 = frame.stack_w2;
                      g_event_log_args[g_event_log_head].w0c = frame.stack_w3;
                      g_event_log_args[g_event_log_head].w10 = frame.stack_w4;
                      g_event_log_args[g_event_log_head].w14 = frame.stack_w5;
                      g_event_log_args[g_event_log_head].w18 = frame.stack_w6;
                      g_event_log_args[g_event_log_head].w1c = frame.stack_w7;
                      phase_or_idx = 0;
                      do {
                        next_i = phase_or_idx + 1;
                        (&g_event_log_data[g_event_log_head].w00)[phase_or_idx] =
                             (&frame.stack_w0)[phase_or_idx];
                        phase_or_idx = next_i;
                      } while (next_i < EVENT_DATA_WORDS);
                      goto LAB_0047b30d;
                    }
                  }
                  else if (((((car[0x27c] == 1) && (tri_mark != TRI_MARK_FFFF0080)) && (0x32 < car[0x27d]))
                           && ((phase_or_idx = race_get_phase(), now_ticks = g_last_frame_ticks,
                               phase_or_idx != 1 && (phase_or_idx != 2)))) &&
                          ((phase_or_idx != 3 && ((phase_or_idx != 4 && (phase_or_idx != 5)))))) {
                    (&g_event_log_type)[g_event_log_head] = 6;
                    (&g_event_log_time)[g_event_log_head] = now_ticks;
                    (&g_event_log_param)[g_event_log_head] = *car + 0xff00;
                    g_event_log_args[g_event_log_head].arg0 = TRI_MARK_FF0000FF;
                    g_event_log_args[g_event_log_head].arg1 = frame.stack_w1;
                    g_event_log_args[g_event_log_head].arg2 = frame.stack_w2;
                    g_event_log_args[g_event_log_head].w0c = frame.stack_w3;
                    g_event_log_args[g_event_log_head].w10 = frame.stack_w4;
                    g_event_log_args[g_event_log_head].w14 = frame.stack_w5;
                    g_event_log_args[g_event_log_head].w18 = frame.stack_w6;
                    g_event_log_args[g_event_log_head].w1c = frame.stack_w7;
                    phase_or_idx = 0;
                    do {
                      next_i = phase_or_idx + 1;
                      (&g_event_log_data[g_event_log_head].w00)[phase_or_idx] =
                           (&frame.stack_w0)[phase_or_idx];
                      phase_or_idx = next_i;
                    } while (next_i < EVENT_DATA_WORDS);
LAB_0047b30d:
                    g_event_log_head = g_event_log_head + 1;
                    if (0xff < g_event_log_head) {
                      g_event_log_head = 0;
                    }
                  }
                  *point_done = 1;
                  g_car_8bd790 = g_car_8bd790 + 1;
                  if (g_car_8bd790 == 4) {
                    return;
                  }
                }
              }
            }
          }
        }
        point_ptr = point_ptr + 3;
        wheel_rec = wheel_rec + 0x31;
        point_done = point_done + 1;
      } while ((char *)point_ptr < (char *)&g_car_8bd790 + 0x4);
      tri_index = tri_index + 1;
      tri = tri + 0x24;
    } while (tri_index < g_collision_triangle_count);
  }
  return;
}


