#include "mfl_tier2.h"
/*004a5d70*/ extern int object_render();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*005e64a4*/ extern float k_f_0;
/*006a26c4*/ extern float g_course_6a26c4;
/*006a26cc*/ extern undefined4 g_course_clumps_6a26cc;
/*006a2724*/ extern float g_course_6a2724;
/*006a272c*/ extern undefined4 g_course_clumps_6a272c;
/*006a2780*/ extern undefined4 g_course_sands_6a2780;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*006a2734*/ extern char g_course_clumps_6a2734[];
/*006a26d4*/ extern char g_course_clumps_6a26d4[];


/* course_sands_cb2 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void course_sands_cb2(char *course)

{
  undefined4 *clump_slot;
  
  if (*(RpClump **)(course + 0xc168) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc168));
  }
  if (*(RpClump **)(course + 0xc114) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc114));
  }
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)0x5);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)0x6);
  RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)0x5);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  if (*(RpClump **)(course + 0xc170) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc170));
  }
  if (g_course_6a26c4 > k_f_0) {
    clump_slot = &g_course_clumps_6a26cc;
    do {
      RpClumpRender((RpClump *)*clump_slot);
      clump_slot = clump_slot + 1;
    } while ((int)clump_slot < ((int)&g_course_clumps_6a26d4 + 0x4));
  }
  if (g_course_6a2724 > k_f_0) {
    clump_slot = &g_course_clumps_6a272c;
    do {
      RpClumpRender((RpClump *)*clump_slot);
      clump_slot = clump_slot + 1;
    } while ((int)clump_slot < ((int)&g_course_clumps_6a2734 + 0x4));
  }
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)0x1);
  object_render(g_course_sands_6a2780);
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)0x2);
  return;
}
