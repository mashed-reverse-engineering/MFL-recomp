#include "mfl_tier2.h"
/*00405e40*/ extern int stub_return_zero();
/*004558d0*/ extern int course_object_render_6a1f28_b();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*0060d8bc*/ extern float g_course_nt1_60d8bc;
/*006a06e0*/ extern undefined4 g_sky_atomic;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* course_nt1_s_cb2 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

/*0044a010*/ extern int course_sky_atomic_render();

/* offsets of RpClump pointers inside the course state block (inferred roles) */
#define COURSE_STATE_PRE_CLUMP_OFFSET (0xc118)
#define COURSE_STATE_POST_CLUMP_OFFSET (0xc11c)

/*
 * Render pass for the NT1 special course (course id COURSE_NT1_S, inferred).
 * course_state: int holding the address of the course state block; its
 * RpClump pointers are read at the offsets above. Draws, in order: the pre clump if set, the
 * course objects, the post clump if set, then the sky atomic with depth
 * writes off. Returns nothing.
 */
void course_nt1_s_cb2(int course_state)

{
  /* pre clump (drawn before the course objects), skipped when NULL */
  if (*(RpClump **)(course_state + COURSE_STATE_PRE_CLUMP_OFFSET) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course_state + COURSE_STATE_PRE_CLUMP_OFFSET));
  }
  course_object_render_6a1f28_b(course_state);
  /* post clump (drawn after the objects), only when the offset is nonzero;
     bracketed by the stub calls (inferred) */
  if (*(int *)(course_state + COURSE_STATE_POST_CLUMP_OFFSET) != 0) {
    stub_return_zero((double)g_course_nt1_60d8bc);
    RpClumpRender(*(RpClump **)(course_state + COURSE_STATE_POST_CLUMP_OFFSET));
    stub_return_zero(0,0);
  }
  /* z write off (value 0 = FALSE, inferred) before the sky draws */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  course_sky_atomic_render();
  return;
}


