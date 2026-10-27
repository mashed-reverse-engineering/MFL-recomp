#include "mfl_tier2.h"
/*004a5d70*/ extern int object_render();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*006a2638*/ extern undefined4 g_course_sandsnow_6a2638;
/*
 * course_sandsnow_cb2: per-frame render callback for the Sands/Snow course (inferred).
 * Renders the RpClump objects stored in course_data at offsets 0xc114..0xc168 (skipping NULL slots),
 * then draws g_course_sandsnow_6a2638 through object_render.
 * course_data: base pointer to the course state block (inferred).
 * Returns nothing.
 */
void course_sandsnow_cb2(struct CourseWorld *course_data)

{
  /* Render each clump slot that is set (inferred: the six slots are the course's scenery objects). */
  if ((RpClump *)course_data->clumps[22] != (RpClump *)0x0) {
    RpClumpRender((RpClump *)course_data->clumps[22]);
  }
  if ((RpClump *)course_data->clumps[1] != (RpClump *)0x0) {
    RpClumpRender((RpClump *)course_data->clumps[1]);
  }
  if ((RpClump *)course_data->clumps[14] != (RpClump *)0x0) {
    RpClumpRender((RpClump *)course_data->clumps[14]);
  }
  if ((RpClump *)course_data->clumps[15] != (RpClump *)0x0) {
    RpClumpRender((RpClump *)course_data->clumps[15]);
  }
  if ((RpClump *)course_data->clumps[16] != (RpClump *)0x0) {
    RpClumpRender((RpClump *)course_data->clumps[16]);
  }
  if ((RpClump *)course_data->clumps[9] != (RpClump *)0x0) {
    RpClumpRender((RpClump *)course_data->clumps[9]);
  }
  object_render(g_course_sandsnow_6a2638);
  return;
}


