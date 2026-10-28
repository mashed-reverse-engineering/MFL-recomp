// cluster-entries: trigger_register_object_type8_cb_4565b0
/* Installed cluster trigger_register_object_type8_cb_4565b0 (tools/cluster_install.py from output/cluster_pilot_kb/trigger_register_object_type8_cb_4565b0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern struct EventLogArgs g_event_log_args[256];
/*0040fdc0*/ extern int race_get_phase();
/*0047a5b0*/ extern int car_add_velocity();
/*0047b520*/ extern int car_current_matrix();
/*0047b6e0*/ extern int car_get_velocity();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd378*/ extern float k_f_0_1;
/*005ddad0*/ extern float k_f_180;
/*005df2b4*/ extern float k_f_5_0en4;
/*007e7f60*/ extern undefined4 g_event_log_head;
/*007e7f64*/ extern undefined4 g_event_log_type;
/*007e8364*/ extern undefined4 g_event_log_time;
/*007e8764*/ extern undefined4 g_event_log_param;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
/* trigger_register_object_type8_cb_4565b0 */

/* function pointer pushed/stored by trigger_register_object_type8 (MOV dword ptr [ESP +
   0x28],0x4565b0) */

void trigger_register_object_type8_cb_4565b0(int param_1, int param_2)

{
  char *trigger_data;
  float push_scale;
  undefined4 frame_ticks;
  int event;
  RwMatrix *object_ltm;
  int car_or_phase;
  RwReal car_speed;
  RwV3d push_vec;
  RwV3d car_velocity;
  RwMatrix inv_ltm;
  
  event = param_2;
  trigger_data = *(char **)(param_1 + 0x14);
  car_or_phase = *(int *)(param_2 + 0x10);
  if ((0xfeff < car_or_phase) && (car_or_phase < 0xff08)) {
    car_or_phase = car_or_phase + -0xff00;
    /* Event ids 0xff00..0xff07 mean car index 0..7 (inferred from the range test). Pushes that car's velocity toward the object, scaled by speed, then stores the object-local push x/z in trigger_data+0x14/+0x1c (inferred). param_1 is reused as the car matrix pointer. */
    car_current_matrix(&param_1,car_or_phase);
    object_ltm = RwFrameGetLTM(*(RwFrame **)(*(char **)(trigger_data + 8) + 4));
    push_vec.x = *(float *)(param_1 + 0x30) - (object_ltm->pos).x;
    push_vec.z = *(float *)(param_1 + 0x38) - (object_ltm->pos).z;
    push_vec.y = 0.0;
    car_get_velocity(&car_velocity,car_or_phase);
    car_speed = RwV3dLength(&car_velocity);
    push_scale = car_speed * k_f_0_1 + k_f_180;
    push_vec.x = push_vec.x * push_scale;
    push_vec.y = push_vec.y * push_scale;
    push_vec.z = push_vec.z * push_scale;
    car_add_velocity(&push_vec,car_or_phase);
    RwMatrixInvert(&inv_ltm,object_ltm);
    RwV3dTransformVectors(&push_vec,&push_vec,1,&inv_ltm);
    *(float *)(trigger_data + 0x14) = push_vec.x * k_f_5_0en4;
    *(float *)(trigger_data + 0x1c) = push_vec.z * k_f_5_0en4;
    car_or_phase = race_get_phase();
    frame_ticks = g_last_frame_ticks;
    /* Log event 0xb for the event id unless the race phase is 1..5 (inferred). */
    if (((car_or_phase != 1) && (((car_or_phase != 2 && (car_or_phase != 3)) && (car_or_phase != 4)))) && (car_or_phase != 5)) {
      (&g_event_log_type)[g_event_log_head] = 0xb;
      (&g_event_log_time)[g_event_log_head] = frame_ticks;
      (&g_event_log_param)[g_event_log_head] = *(undefined4 *)(event + 0x10);
      g_event_log_args[g_event_log_head].arg0 = 1;
      g_event_log_head = g_event_log_head + 1;
      if (0xff < g_event_log_head) { /* unsigned (jb) */
        g_event_log_head = 0;
      }
    }
  }
  return;
}

