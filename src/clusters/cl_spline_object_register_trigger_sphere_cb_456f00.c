// cluster-entries: spline_object_register_trigger_sphere_cb_456f00
/* Installed cluster spline_object_register_trigger_sphere_cb_456f00 (tools/cluster_install.py from output/cluster_pilot_kb/spline_object_register_trigger_sphere_cb_456f00.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern struct EventLogArgs g_event_log_args[256];
/*0040fdc0*/ extern int race_get_phase();
/*0047e640*/ extern int __cdecl car_collision_impulse(int p0, struct RwBBox *p1, RwMatrix *p2, float p3, float *p4);
/*004a1680*/ extern int player_rumble();
/*005df578*/ extern float k_f_1_25en7;
/*007e7f60*/ extern undefined4 g_event_log_head;
/*007e7f64*/ extern undefined4 g_event_log_type;
/*007e8364*/ extern undefined4 g_event_log_time;
/*007e8764*/ extern undefined4 g_event_log_param;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
/* spline_object_register_trigger_sphere_cb_456f00 */

/* function pointer pushed/stored by spline_object_register_trigger_sphere (MOV dword ptr [ESP +
   0x20],0x456f00) */

void spline_object_register_trigger_sphere_cb_456f00(int trigger, int hit)

{
  undefined4 now_ticks;
  int id_or_result;
  int sphere_or_phase;
  struct {
    float impulse_xy [2];
    float impulse_z;
  } frame;
  
  sphere_or_phase = *(int *)(trigger + 0x14);
  id_or_result = *(int *)(hit + 0x10);
  if ((0xfeff < id_or_result) && (id_or_result < 0xff08)) {
    id_or_result = car_collision_impulse
                      (id_or_result + -0xff00,(RwBBox *)(sphere_or_phase + 0x20),
                       (RwMatrix *)(*(int *)(*(int *)(sphere_or_phase + 4) + 4) + 0x10),1000.0,frame.impulse_xy);
    if (id_or_result != 0) {
      *(float *)(sphere_or_phase + 0xc) = frame.impulse_z * k_f_1_25en7 + *(float *)(sphere_or_phase + 0xc);
      sphere_or_phase = race_get_phase();
      now_ticks = g_last_frame_ticks;
      if ((((sphere_or_phase != 1) && (sphere_or_phase != 2)) && (sphere_or_phase != 3)) && ((sphere_or_phase != 4 && (sphere_or_phase != 5)))) {
        (&g_event_log_type)[g_event_log_head] = 0xb;
        (&g_event_log_time)[g_event_log_head] = now_ticks;
        (&g_event_log_param)[g_event_log_head] = *(undefined4 *)(hit + 0x10);
        g_event_log_args[g_event_log_head].arg0 = 0;
        g_event_log_head = g_event_log_head + 1;
        if (0xff < g_event_log_head) { /* unsigned (jb) */
          g_event_log_head = 0;
        }
      }
      player_rumble(*(int *)(hit + 0x10) + -0xff00,1,2,0xff);
    }
  }
  return;
}

