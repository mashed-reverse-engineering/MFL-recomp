// cluster-entries: course_storm_cb0_sub_4aaa60
/* Installed cluster course_storm_cb0_sub_4aaa60 (tools/cluster_install.py from output/cluster_pilot_kb/course_storm_cb0_sub_4aaa60.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*00556360*/ extern RwReal RpSplineFindMatrix(struct RpSpline *p0, RwInt32 p1, RwReal p2, struct RwV3d *p3, RwMatrix *p4);
/*005dd370*/ extern float k_f_0_3;
/*005dd628*/ extern float k_f_0_25;
/* rounding point: the original spills the parameter span (end - start) to a float before scaling it */
static float float_barrier(float value)
{
  return value;
}

/* course_storm_cb0_sub_4aaa60 */

/* single caller course_storm_cb0_sub_4aac50 */

/* Compute a bounding sphere for the spline segment [spline_obj[1], spline_obj[2]]: centre = mean of 4 sampled matrix positions (ends and 0.3-span inner points), radius = max distance to them; stored at spline_obj[8..0xb], centre copied to [0xc..0xe] (inferred). scratch_y/scratch_z hold t values first, then centre y/z. */
void course_storm_cb0_sub_4aaa60(undefined4 *spline_obj)

{
  float scratch_y;
  float scratch_z;
  float center_x;
  int corners_left;
  RwReal *corner_pos_ptr;
  RwReal dist;
  float max_radius;
  RwV3d to_corner;
  RwV3d up_axis;
  RwMatrix mat_start;
  RwMatrix mat_end;
  RwMatrix mat_start_inner;
  RwMatrix mat_end_inner;
  
  scratch_y = *(float *)&spline_obj[2];
  scratch_z = *(float *)&spline_obj[1];
  up_axis.x = 0.0;
  up_axis.y = 1.0;
  up_axis.z = 0.0;
  max_radius = 0.0;
  RpSplineFindMatrix((RpSpline *)*spline_obj,10,*(float *)&spline_obj[1],&up_axis,&mat_start);
  RpSplineFindMatrix((RpSpline *)*spline_obj,10,*(float *)&spline_obj[2],&up_axis,&mat_end);
  scratch_y = float_barrier(scratch_y - scratch_z) * k_f_0_3;
  RpSplineFindMatrix((RpSpline *)*spline_obj,10,scratch_y + *(float *)&spline_obj[1],&up_axis,&mat_start_inner);
  RpSplineFindMatrix((RpSpline *)*spline_obj,10,*(float *)&spline_obj[2] - scratch_y,&up_axis,&mat_end_inner);
  corner_pos_ptr = &mat_start.pos.y;
  corners_left = 4;
  center_x = (mat_end_inner.pos.x + mat_start_inner.pos.x + mat_end.pos.x + mat_start.pos.x) * k_f_0_25;
  scratch_y = (mat_end_inner.pos.y + mat_start_inner.pos.y + mat_end.pos.y + mat_start.pos.y) * k_f_0_25;
  scratch_z = (mat_end_inner.pos.z + mat_start_inner.pos.z + mat_end.pos.z + mat_start.pos.z) * k_f_0_25;
  do {
    to_corner.x = center_x - ((RwV3d *)(corner_pos_ptr + -1))->x;
    to_corner.y = scratch_y - *corner_pos_ptr;
    to_corner.z = scratch_z - corner_pos_ptr[1];
    dist = RwV3dLength(&to_corner);
    if (max_radius < dist) {
      max_radius = dist;
    }
    corner_pos_ptr = corner_pos_ptr + 0x10;
    corners_left = corners_left + -1;
  } while (corners_left != 0);
  *(float *)&spline_obj[8] = center_x;
  *(float *)&spline_obj[9] = scratch_y;
  *(float *)&spline_obj[10] = scratch_z;
  *(float *)&spline_obj[0xb] = max_radius;
  *(float *)&spline_obj[0xc] = center_x;
  *(float *)&spline_obj[0xd] = scratch_y;
  *(float *)&spline_obj[0xe] = scratch_z;
  return;
}

