#include "mfl_tier2.h"

#define COURSE_SKY_CLUMPS_END ((int)&g_course_world_field8_0xc100)

/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*00668c18*/ extern struct CourseWorld g_course_world;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00674d18*/ extern char g_course_world_field8_0xc100[];


/* course_sky_clumps_render */

/* Renders active course sky clumps with source/destination alpha blending. */

void course_sky_clumps_render(void)
{
  uint *sky_clump_ptr;
  
  sky_clump_ptr = g_course_world.sky_clumps;
  do {
    /* render valid sky clumps with alpha blending */
    if (*sky_clump_ptr != 0) {
      RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
      RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
      RpClumpRender((RpClump *)*sky_clump_ptr);
    }
    sky_clump_ptr = sky_clump_ptr + 1;
  } while ((int)sky_clump_ptr < COURSE_SKY_CLUMPS_END);
  return;
}
