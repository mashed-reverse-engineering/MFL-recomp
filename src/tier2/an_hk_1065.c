#include "mfl_tier2.h"
/*005dd814*/ extern float k_f_6;
/*00801b14*/ extern char *g_rw_801b14;
/* car_probe_ground_distance_cb_47b420 */

/* function pointer pushed/stored by car_probe_ground_distance (PUSH 0x47b420) */

/* TIER2: unvalidated 2026-10-27 authority=static */
void car_probe_ground_distance_cb_47b420(char *unused_1,char *hit_mesh,char *hit_record,float hit_fraction,float *best_dist)
{
  int surface_colour;
  
  if (hit_fraction * k_f_6 < *best_dist) {
    surface_colour = *(int *)(*(char **)(*(char **)(g_rw_801b14 + 0x10) +
                             (uint)*(ushort *)
                                    (*(char **)(hit_mesh + 4) + 6 + *(int *)(hit_record + 0x18) * 8) * 4)
                    + 4);
    if ((surface_colour != -0xffff01) && (surface_colour != -0xfefeff)) {
      *best_dist = hit_fraction * k_f_6;
    }
  }
  return;
}


