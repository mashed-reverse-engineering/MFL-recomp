// cluster-entries: course_city_cb1_sub_455e60
/* Installed cluster course_city_cb1_sub_455e60 (tools/cluster_install.py from output/cluster_pilot_md/course_city_cb1_sub_455e60.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0049d090*/ extern void __cdecl fx_spawn_burst_spread(undefined4 p0, int count, float p2, float *position, float p4, unsigned char *colour, float *velocity, float p7);
/*00461cc0*/ extern void __cdecl weapon_particle_spark_emit(undefined4 p0, float p1);
/* course_city_cb1_sub_455e60 */

/* "single caller course_city_cb1: a burst of 25 grey sparks from the object in esi, then one spark emit" */
void course_city_cb1_sub_455e60(undefined4 in_esi)
{
  struct {
    unsigned char colour[4];
    float velocity[3];
    float position[3];
  } frame;

  frame.colour[0] = 0x80;
  frame.colour[1] = 0x80;
  frame.colour[2] = 0xff;
  frame.colour[3] = 0xff;
  frame.position[0] = 0.0f;
  frame.position[1] = 1.0f;
  frame.position[2] = 0.0f;
  frame.velocity[0] = 0.0f;
  frame.velocity[1] = -0.001f;
  frame.velocity[2] = 0.0f;
  fx_spawn_burst_spread(in_esi, 0x19, 0.06f, frame.position, 45.0f, frame.colour, frame.velocity, 10.0f);
  weapon_particle_spark_emit(in_esi, 1.0f);
}

