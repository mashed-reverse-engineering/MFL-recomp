#include "mfl_tier2.h"
/*0044ef80*/ extern double camera_get_8c7e0c(void);
/*004568d0*/ extern int course_quad_render_a(void *p0);
/*004569f0*/ extern undefined __cdecl course_quad_render_b(void *p0);
/*004577e0*/ extern int course_objects_render();
/*0049c8f0*/ extern int particle_slot_call_render_method();
/*004a5d70*/ extern int object_render();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*005df5c0*/ extern float k_f_49;
/*005df5c4*/ extern float k_f_35;
/*006a262c*/ extern undefined4 g_course_rouabout_6a262c;
/*006a2630*/ extern undefined4 g_course_rouabout_6a2630;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c53e0*/ extern undefined4 g_course_8c53e0;
/* course_rouabout_cb2 */
/*
 * Per-frame draw callback for the Rouabout course (cb2 = second course callback, inferred).
 * Draws the course's RpClump slots, the two objects, then the quad and object pass
 * only when the camera value is inside (k_f_35, k_f_49).
 * Parameter: course - per-course state block; RpClump pointers at +0xc110..+0xc170 (inferred).
 * Returns nothing.
 */
void course_rouabout_cb2(char *course)
{
  double camera_value_wide;
  float camera_value;

  camera_value_wide = (double)camera_get_8c7e0c();
  camera_value = (float)camera_value_wide;
  /* first clump slot (+0xc164), drawn before the render state setup */
  if (COURSE_CLUMP((CourseWorld *)course, 21) != (RpClump *)0x0) {
    RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 21));
  }
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
  /* blended pass: no depth write, no fog, src-alpha / inv-src-alpha blending (inferred) */
  particle_slot_call_render_method(0);
  /* restore the normal state: fog on, depth write on, back-face culling off (inferred) */
  RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
  if (COURSE_CLUMP((CourseWorld *)course, 3) != (RpClump *)0x0) {
    RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 3));
  }
  if (COURSE_CLUMP((CourseWorld *)course, 0) != (RpClump *)0x0) {
    RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 0));
  }
  if (COURSE_CLUMP((CourseWorld *)course, 5) != (RpClump *)0x0) {
    RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 5));
  }
  if (COURSE_CLUMP((CourseWorld *)course, 11) != (RpClump *)0x0) {
    RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 11));
  }
  if (COURSE_CLUMP((CourseWorld *)course, 24) != (RpClump *)0x0) {
    RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 24));
  }
  if (COURSE_CLUMP((CourseWorld *)course, 9) != (RpClump *)0x0) {
    RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 9));
  }
  if (COURSE_CLUMP((CourseWorld *)course, 8) != (RpClump *)0x0) {
    RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 8));
  }
  object_render(g_course_rouabout_6a2630);
  object_render(g_course_rouabout_6a262c);
  if (COURSE_CLUMP((CourseWorld *)course, 7) != (RpClump *)0x0) {
    RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 7));
  }
  if (COURSE_CLUMP((CourseWorld *)course, 6) != (RpClump *)0x0) {
    RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 6));
  }
  /* only when the camera value lies strictly between k_f_35 and k_f_49 (inferred range check) */
  if ((camera_value > k_f_35) && (camera_value < k_f_49)) {
    course_quad_render_a(&g_course_8c53e0);
    course_quad_render_b(&g_course_8c53e0);
    course_objects_render();
    return;
  }
  return;
}
