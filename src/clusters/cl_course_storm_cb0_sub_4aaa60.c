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

void course_storm_cb0_sub_4aaa60(undefined4 *spline_obj)

{
  float fVar1;
  float t_start;
  float fVar3;
  int iVar4;
  RwReal *pRVar5;
  RwReal RVar6;
  float unused_f;
  RwV3d RStack_118;
  RwV3d up_axis;
  RwMatrix mat_start;
  RwMatrix mat_end;
  RwMatrix mat_start_inner;
  RwMatrix mat_end_inner;
  
  fVar1 = *(float *)&spline_obj[2];
  t_start = *(float *)&spline_obj[1];
  up_axis.x = 0.0;
  up_axis.y = 1.0;
  up_axis.z = 0.0;
  unused_f = 0.0;
  RpSplineFindMatrix((RpSpline *)*spline_obj,10,*(float *)&spline_obj[1],&up_axis,&mat_start);
  RpSplineFindMatrix((RpSpline *)*spline_obj,10,*(float *)&spline_obj[2],&up_axis,&mat_end);
  fVar1 = float_barrier(fVar1 - t_start) * k_f_0_3;
  RpSplineFindMatrix((RpSpline *)*spline_obj,10,fVar1 + *(float *)&spline_obj[1],&up_axis,&mat_start_inner);
  RpSplineFindMatrix((RpSpline *)*spline_obj,10,*(float *)&spline_obj[2] - fVar1,&up_axis,&mat_end_inner);
  pRVar5 = &mat_start.pos.y;
  iVar4 = 4;
  fVar3 = (mat_end_inner.pos.x + mat_start_inner.pos.x + mat_end.pos.x + mat_start.pos.x) * k_f_0_25;
  fVar1 = (mat_end_inner.pos.y + mat_start_inner.pos.y + mat_end.pos.y + mat_start.pos.y) * k_f_0_25;
  t_start = (mat_end_inner.pos.z + mat_start_inner.pos.z + mat_end.pos.z + mat_start.pos.z) * k_f_0_25;
  do {
    RStack_118.x = fVar3 - ((RwV3d *)(pRVar5 + -1))->x;
    RStack_118.y = fVar1 - *pRVar5;
    RStack_118.z = t_start - pRVar5[1];
    RVar6 = RwV3dLength(&RStack_118);
    if (unused_f < RVar6) {
      unused_f = RVar6;
    }
    pRVar5 = pRVar5 + 0x10;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  *(float *)&spline_obj[8] = fVar3;
  *(float *)&spline_obj[9] = fVar1;
  *(float *)&spline_obj[10] = t_start;
  *(float *)&spline_obj[0xb] = unused_f;
  *(float *)&spline_obj[0xc] = fVar3;
  *(float *)&spline_obj[0xd] = fVar1;
  *(float *)&spline_obj[0xe] = t_start;
  return;
}

