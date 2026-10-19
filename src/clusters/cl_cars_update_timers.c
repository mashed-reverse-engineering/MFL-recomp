// cluster-entries: cars_update_timers
/* Installed cluster cars_update_timers (tools/cluster_install.py from output/cluster_pilot_zy/cars_update_timers.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*008b06e0*/ extern struct Car player_car[16];
/*007e7f60*/ extern volatile unsigned int g_event_log_head;  /* volatile: the stores through (&g_event_log_type)[head] may hit the head itself (the original reloads it) */
/*007e7f64*/ extern volatile int g_event_log_type;
/*007e8364*/ extern volatile int g_event_log_time;
/*007e8764*/ extern volatile int g_event_log_param;
/*007ef818*/ extern int g_last_frame_ticks;
/*0040fdc0*/ extern int race_get_phase(void);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *out, struct RwV3d *in);
/*005e64a4*/ extern float k_f_0;
/*005dd340*/ extern float k_f_0_5;

/* Per player car (4), per effect kind (6): count the hold time down by dt ms; once a hold is used up the matching effect level drops by dt (to 0).  Also logs event 7 (param 0xff00 + car) when the car is moving at a flat angle (0 < cos < 0.5) to its heading outside the race phases 1..5. */
void cars_update_timers(unsigned int dt)
{
  float step = (float)dt;
  int car;
  int k;
  float after;
  float cos_a;
  int phase;
  struct RwV3d dir;

  for (car = 0; car < 4; car++) {
    for (k = 0; k < 6; k++) {
      if (player_car[car].effect_hold[k] > k_f_0) {
        after = player_car[car].effect_hold[k] - step;
        player_car[car].effect_hold[k] = after;
        if (after < k_f_0) {
          player_car[car].effect_hold[k] = 0.0f;
        }
      }
      if (player_car[car].effect_hold[k] == k_f_0) {
        if (dt < player_car[car].effect_level[k]) {
          player_car[car].effect_level[k] = player_car[car].effect_level[k] - dt;
        } else {
          player_car[car].effect_level[k] = 0;
        }
      }
    }
    if (player_car[car].velocity[0] != k_f_0 || player_car[car].velocity[1] != k_f_0 || player_car[car].velocity[2] != k_f_0) {
      RwV3dNormalize(&dir, (struct RwV3d *)player_car[car].velocity);
      cos_a = player_car[car].forward[2] * dir.z + player_car[car].forward[0] * dir.x + player_car[car].forward[1] * dir.y;
      if (cos_a < k_f_0_5 && k_f_0 < cos_a) {
        phase = race_get_phase();
        if (phase != 1 && phase != 2 && phase != 3 && phase != 4 && phase != 5) {
          (&g_event_log_type)[g_event_log_head] = 7;
          (&g_event_log_time)[g_event_log_head] = g_last_frame_ticks;
          (&g_event_log_param)[g_event_log_head] = car + 0xff00;
          g_event_log_head = g_event_log_head + 1;
          if (g_event_log_head > 0xff) {
            g_event_log_head = 0;
          }
        }
      }
    }
  }
}

