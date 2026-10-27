#include "mfl_tier2.h"
/*0049c8f0*/ extern int particle_slot_call_render_method();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;


/* course_training_cb10 */

/* Course Training render callback: draws particles (z-write off), then the clump at +0xc140 with back-face culling disabled. */
void course_training_cb10(char *course)
{
  void *pvStack_4;
  
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  particle_slot_call_render_method(0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  RwRenderStateGet(rwRENDERSTATECULLMODE,&pvStack_4);
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)0x2);
  if (*(RpClump **)(course + 0xc140) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc140));
  }
  RwRenderStateSet(rwRENDERSTATECULLMODE,pvStack_4);
  return;
}
