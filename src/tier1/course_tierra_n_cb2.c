#include "mfl_tier2.h"
/*004577e0*/ extern int course_objects_render();
/*004a5d70*/ extern int object_render();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*006a2658*/ extern undefined4 g_course_tierra_6a2658;
/*006a2660*/ extern undefined4 g_course_tierra_6a2660;
/*006a2664*/ extern undefined4 g_course_tierra_6a2664;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;


/* Renders the Tierra course: its clump slots (fields at +0xc110..+0xc168 of the course
   struct, names unknown), a pass with alpha blending and fog off/on for the
   objects, then restores back-face culling. */
void course_tierra_n_cb2(char *course)
{
  /* clump at +0xc168, drawn first with the default render state */
  if (*(RpClump **)(course + 0xc168) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc168));
  }
  /* state for the translucent pass: z-write off, fog off, src-alpha / inv-src-alpha blend */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  /* z-write back on, culling off for the clump pass */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
  if (*(RpClump **)(course + 0xc11c) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc11c));
  }
  if (*(RpClump **)(course + 0xc110) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc110));
  }
  if (*(RpClump **)(course + 0xc124) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc124));
  }
  if (*(RpClump **)(course + 0xc13c) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc13c));
  }
  if (*(RpClump **)(course + 0xc134) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc134));
  }
  if (*(RpClump **)(course + 0xc130) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc130));
  }
  course_objects_render();
  object_render(g_course_tierra_6a2658);
  object_render(g_course_tierra_6a2660);
  object_render(g_course_tierra_6a2664);
  /* back-face culling restored (2 = rwCULLMODECULLBACK) */
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLBACK);
  return;
}
