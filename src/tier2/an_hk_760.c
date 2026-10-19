#include "mfl_tier2.h"
/*00405e40*/ extern int stub_return_zero();
/*004558d0*/ extern int course_object_render_6a1f28_b();
/*004a5d70*/ extern int object_render();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*0060d8bc*/ extern float g_course_nt1_60d8bc;
/*006a1f2c*/ extern undefined4 g_course_nt1_6a1f2c;

/* Byte offsets into the course state. Roles inferred from use only:
   A = clump rendered first, B = clump rendered after the stub call (inferred). */
#define COURSE_CLUMP_A_OFFSET (0xc118)
#define COURSE_CLUMP_B_OFFSET (0xc11c)

/* course_nt1_cb2
 * Per-frame course draw step. Renders clump A if set, then the course objects,
 * then clump B if set (with stub_return_zero calls around it; stub role inferred from name).
 * course_state: address of the course state block, passed as int (inferred).
 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void course_nt1_cb2(int course_state)

{
  /* Clump A: render only when the pointer is non-null. */
  if (*(RpClump **)(course_state + COURSE_CLUMP_A_OFFSET) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course_state + COURSE_CLUMP_A_OFFSET));
  }
  course_object_render_6a1f28_b(course_state);
  object_render(g_course_nt1_6a1f2c);
  /* Clump B: render only when its slot is non-zero. */
  if (*(int *)(course_state + COURSE_CLUMP_B_OFFSET) != 0) {
    stub_return_zero((double)g_course_nt1_60d8bc);
    RpClumpRender(*(RpClump **)(course_state + COURSE_CLUMP_B_OFFSET));
    stub_return_zero(0,0);
  }
  return;
}


