#include "mfl_tier2.h"
/*0049b700*/ extern int fx_ptank_render_b();
/*004a5d70*/ extern int object_render();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*006a2150*/ extern undefined4 g_course_city_6a2150;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

/* byte offsets into the course state of the three RpClump pointers rendered by course_city_cb2 (roles unknown) */
#define STATE_CLUMP_0_OFFSET (0xc138)
#define STATE_CLUMP_1_OFFSET (0xc13c)
#define STATE_CLUMP_2_OFFSET (0xc144)

/* course_city_cb2
 * Course-city render callback (inferred): draws the course's effects and clumps with depth
 * writes off for the particle pass, then the three RpClumpRender objects and object_render.
 * course_state: course state struct (inferred), the RpClump pointers are read from it.
 * Returns nothing.
 */


void course_city_cb2(char *course_state)

{
  /* particle pass: depth test on, depth write off */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  fx_ptank_render_b();
  /* restore depth write for the clump pass */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  /* render the three clumps stored in the course state */
  RpClumpRender(*(RpClump **)(course_state + STATE_CLUMP_0_OFFSET));
  RpClumpRender(*(RpClump **)(course_state + STATE_CLUMP_1_OFFSET));
  RpClumpRender(*(RpClump **)(course_state + STATE_CLUMP_2_OFFSET));
  object_render(g_course_city_6a2150);
  return;
}


