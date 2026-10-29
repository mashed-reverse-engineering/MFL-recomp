#include "mfl_tier2.h"
/*004a5d70*/ extern int object_render();
/*006a2668*/ extern undefined4 g_data_6a2668;
/*006a266c*/ extern undefined4 g_data_6a266c;
/*006a2670*/ extern undefined4 g_data_6a2670;
/*006a2674*/ extern undefined4 g_data_6a2674;
/*006a2678*/ extern undefined4 g_data_6a2678;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* course_forest_cb2_sub_458f20 */

/* single caller course_forest_cb2 */

void course_forest_cb2_sub_458f20(void)

{
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)0x1);
  object_render(g_data_6a2668);
  object_render(g_data_6a2674);
  object_render(g_data_6a2678);
  object_render(g_data_6a266c);
  object_render(g_data_6a2670);
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)0x2);
  return;
}
