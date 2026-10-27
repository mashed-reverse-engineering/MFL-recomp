#include "mfl_tier2.h"
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);


#define COURSE_FOREST_CLUMP_OFFSET 0xc110

/* Render callback for forest course: renders course clump if present, then calls base handler. */
void course_forest_cb2(char *course_ctx)
{
  /* Render the forest clump if present */
  if (*(RpClump **)(course_ctx + COURSE_FOREST_CLUMP_OFFSET) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course_ctx + COURSE_FOREST_CLUMP_OFFSET));
  }
  /* Call unknown base course handler */
  func_0x00458f20(course_ctx);
  return;
}
