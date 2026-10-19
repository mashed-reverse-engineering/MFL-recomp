#include "mfl_tier2.h"
extern struct EventLogArgs g_event_log_args[256];
extern struct EventLogData g_event_log_data[256];
/*0040fdc0*/ extern int race_get_phase();
/*00477130*/ extern int __cdecl car_wheel_contact_find_index(undefined4 p0, undefined4 p1);
/*005dd370*/ extern float k_f_0_3;
/*005dd628*/ extern float k_f_0_25;
/*005ddc28*/ extern float k_f_0;
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
/* car_wheels_vs_triangles */

/* "wheel points against the collected triangle list in 0x8BD720 (inferred)" */

void car_wheels_vs_triangles(int *param_1,float *param_2)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  undefined4 uVar16;
  int race_phase;
  int iVar17;
  float *local_90;
  float *local_8c;
  int *local_88;
  uint local_74;
  struct {
    float fStack_40;
    uint local_3c;
    uint local_38;
    uint local_34;
    uint local_30;
    uint local_2c;
    uint local_28;
    uint local_24;
  } frame;
  
  local_74 = 0;
  if (g_collision_triangle_count != 0) {
    do {
      fVar1 = *param_2;
      fVar2 = param_2[1];
      fVar3 = param_2[2];
      fVar4 = param_2[3];
      fVar5 = param_2[4];
      fVar6 = param_2[5];
      fVar7 = param_2[6];
      fVar8 = param_2[7];
      fVar9 = param_2[8];
      fVar10 = param_2[9];
      fVar11 = param_2[10];
      fVar12 = param_2[0xb];
      local_90 = (float *)(param_1 + 0x65);
      local_88 = g_car_8bd720;
      local_8c = (float *)&g_car_8bd764;
      do {
        if (*local_88 == 0) {
          fVar13 = local_8c[-1];
          fVar14 = *local_8c;
          fVar15 = local_8c[1];
          if ((k_f_0 <= (fVar13 - fVar1) * param_2[0x1b] +
                        (fVar14 - fVar2) * param_2[0x1c] + (fVar15 - fVar3) * param_2[0x1d]) &&
             (k_f_0 <= (fVar13 - fVar4) * param_2[0x1e] +
                       (fVar14 - fVar5) * param_2[0x1f] + (fVar15 - fVar6) * param_2[0x20])) {
            if ((k_f_0 <= (fVar13 - fVar7) * param_2[0x21] +
                          (fVar14 - fVar8) * param_2[0x22] + (fVar15 - fVar9) * param_2[0x23]) &&
               (((fVar13 = (fVar13 - fVar7) * fVar10 +
                           (fVar14 - fVar8) * fVar11 + (fVar15 - fVar9) * fVar12,
                 k_f_n0_25 <= fVar13 && (fVar13 < k_f_0_25)) &&
                (k_f_0_3 < fVar10 * *(float *)&param_1[0x272] +
                           fVar11 * *(float *)&param_1[0x273] + fVar12 * *(float *)&param_1[0x274])))) {
              fVar14 = param_2[0xd];
              if (fVar14 == -1.7147562e+38) {
                if (((((param_1[0x27c] == 0) &&
                      (race_phase = race_get_phase(), uVar16 = g_last_frame_ticks, race_phase != 1))
                     && (race_phase != 2)) && ((race_phase != 3 && (race_phase != 4)))) &&
                   (race_phase != 5)) {
                  (&g_event_log_type)[g_event_log_head] = 6;
                  (&g_event_log_time)[g_event_log_head] = uVar16;
                  (&g_event_log_param)[g_event_log_head] = *param_1 + 0xff00;
                  g_event_log_args[g_event_log_head].arg0 = 0xff010101;
                  g_event_log_args[g_event_log_head].arg1 = frame.local_3c;
                  g_event_log_args[g_event_log_head].arg2 = frame.local_38;
                  g_event_log_args[g_event_log_head].w0c = frame.local_34;
                  g_event_log_args[g_event_log_head].w10 = frame.local_30;
                  g_event_log_args[g_event_log_head].w14 = frame.local_2c;
                  g_event_log_args[g_event_log_head].w18 = frame.local_28;
                  g_event_log_args[g_event_log_head].w1c = frame.local_24;
                  race_phase = 0;
                  do {
                    iVar17 = race_phase + 1;
                    (&g_event_log_data[g_event_log_head].w00)[race_phase] = (&frame.fStack_40)[race_phase]
                    ;
                    race_phase = iVar17;
                  } while (iVar17 < 0x10);
                  g_event_log_head = g_event_log_head + 1;
                  if (0xff < g_event_log_head) {
                    g_event_log_head = 0;
                  }
                }
              }
              else {
                race_phase = car_wheel_contact_find_index((undefined4)param_2,(undefined4)param_1);
                if (race_phase == 0) {
                  local_90[0x1b] = param_2[9];
                  local_90[0x1c] = param_2[10];
                  local_90[0x1d] = param_2[0xb];
                  *local_90 = fVar13;
                  local_90[0x16] = param_2[0xc];
                  local_90[0x17] = param_2[0xd];
                  if (fVar14 == -1.7014636e+38) {
                    if (((((param_1[0x27c] == 0) || (param_1[0x27c] == 1)) &&
                         ((race_phase = race_get_phase(), uVar16 = g_last_frame_ticks,
                          race_phase != 1 && ((race_phase != 2 && (race_phase != 3)))))) &&
                        (race_phase != 4)) && (race_phase != 5)) {
                      (&g_event_log_type)[g_event_log_head] = 6;
                      (&g_event_log_time)[g_event_log_head] = uVar16;
                      (&g_event_log_param)[g_event_log_head] = *param_1 + 0xff00;
                      g_event_log_args[g_event_log_head].arg0 = 0xff0000ff;
                      g_event_log_args[g_event_log_head].arg1 = frame.local_3c;
                      g_event_log_args[g_event_log_head].arg2 = frame.local_38;
                      g_event_log_args[g_event_log_head].w0c = frame.local_34;
                      g_event_log_args[g_event_log_head].w10 = frame.local_30;
                      g_event_log_args[g_event_log_head].w14 = frame.local_2c;
                      g_event_log_args[g_event_log_head].w18 = frame.local_28;
                      g_event_log_args[g_event_log_head].w1c = frame.local_24;
                      race_phase = 0;
                      do {
                        iVar17 = race_phase + 1;
                        (&g_event_log_data[g_event_log_head].w00)[race_phase] =
                             (&frame.fStack_40)[race_phase];
                        race_phase = iVar17;
                      } while (iVar17 < 0x10);
                      goto LAB_0047b30d;
                    }
                  }
                  else if (((((param_1[0x27c] == 1) && (fVar14 != -NAN)) && (0x32 < param_1[0x27d]))
                           && ((race_phase = race_get_phase(), uVar16 = g_last_frame_ticks,
                               race_phase != 1 && (race_phase != 2)))) &&
                          ((race_phase != 3 && ((race_phase != 4 && (race_phase != 5)))))) {
                    (&g_event_log_type)[g_event_log_head] = 6;
                    (&g_event_log_time)[g_event_log_head] = uVar16;
                    (&g_event_log_param)[g_event_log_head] = *param_1 + 0xff00;
                    g_event_log_args[g_event_log_head].arg0 = 0xff0000ff;
                    g_event_log_args[g_event_log_head].arg1 = frame.local_3c;
                    g_event_log_args[g_event_log_head].arg2 = frame.local_38;
                    g_event_log_args[g_event_log_head].w0c = frame.local_34;
                    g_event_log_args[g_event_log_head].w10 = frame.local_30;
                    g_event_log_args[g_event_log_head].w14 = frame.local_2c;
                    g_event_log_args[g_event_log_head].w18 = frame.local_28;
                    g_event_log_args[g_event_log_head].w1c = frame.local_24;
                    race_phase = 0;
                    do {
                      iVar17 = race_phase + 1;
                      (&g_event_log_data[g_event_log_head].w00)[race_phase] =
                           (&frame.fStack_40)[race_phase];
                      race_phase = iVar17;
                    } while (iVar17 < 0x10);
LAB_0047b30d:
                    g_event_log_head = g_event_log_head + 1;
                    if (0xff < g_event_log_head) {
                      g_event_log_head = 0;
                    }
                  }
                  *local_88 = 1;
                  g_car_8bd790 = g_car_8bd790 + 1;
                  if (g_car_8bd790 == 4) {
                    return;
                  }
                }
              }
            }
          }
        }
        local_8c = local_8c + 3;
        local_90 = local_90 + 0x31;
        local_88 = local_88 + 1;
      } while ((int)local_8c < (int)&g_car_8bd790 + 0x4);
      local_74 = local_74 + 1;
      param_2 = param_2 + 0x24;
    } while (local_74 < g_collision_triangle_count);
  }
  return;
}


