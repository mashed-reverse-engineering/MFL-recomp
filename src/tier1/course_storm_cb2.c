#include "mfl_tier2.h"
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;


#ifndef NULL
#define NULL 0
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#define COURSE_STORM_OFFSET_UNKNOWN_CLUMP_1 0xc110
#define COURSE_STORM_OFFSET_UNKNOWN_CLUMP_2 0xc114


/* course_storm_cb2: Render callback 2 for storm course, rendering clumps at unknown offsets and storm effects. */

void course_storm_cb2(char *course)
{
  /* Render first clump if present (unknown clump) */
  if (*(RpClump **)(course + COURSE_STORM_OFFSET_UNKNOWN_CLUMP_1) != (RpClump *)NULL) {
    RpClumpRender(*(RpClump **)(course + COURSE_STORM_OFFSET_UNKNOWN_CLUMP_1));
  }
  /* Disable depth writes before rendering unknown effect (func_0x004ab140) */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)FALSE);
  func_0x004ab140();
  /* Re-enable depth writes and disable backface culling to render second clump (unknown clump) */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)TRUE);
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
  RpClumpRender(*(RpClump **)(course + COURSE_STORM_OFFSET_UNKNOWN_CLUMP_2));
  /* Restore default backface culling */
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLBACK);
  return;
}
