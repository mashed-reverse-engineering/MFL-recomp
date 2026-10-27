#include "mfl_tier2.h"
/*0049c8f0*/ extern int particle_slot_call_render_method();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;


/* course_training_cb10 */

void course_training_cb10(char *param_1)
{
  void *pvStack_4;
  
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  particle_slot_call_render_method(0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  RwRenderStateGet(rwRENDERSTATECULLMODE,&pvStack_4);
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)0x2);
  if (*(RpClump **)(param_1 + 0xc140) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(param_1 + 0xc140));
  }
  RwRenderStateSet(rwRENDERSTATECULLMODE,pvStack_4);
  return;
}
