// cluster-entries: course_get_record_point
/* Installed cluster course_get_record_point (tools/cluster_install.py from output/cluster_pilot_kd/course_get_record_point.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern char g_course_lap_data_field49624_0xc1d8[];
/* course_get_record_point */

/* "(i, j): 0x681840 + i * 0x4C + j * 0xC" */

int course_get_record_point(int param_1, int param_2)

{
  return param_1 * 0x4c + ((int)&g_course_lap_data_field49624_0xc1d8) + param_2 * 0xc;
}

