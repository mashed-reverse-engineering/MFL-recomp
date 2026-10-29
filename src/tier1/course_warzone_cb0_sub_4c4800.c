#include "mfl_tier2.h"
/*004c47c0*/ extern int gap_4c47c0();
/*004d1420*/ extern struct RwFrame * RwFrameForAllChildren(struct RwFrame *p0, RwFrameCallBack p1, void *p2);
/* course_warzone_cb0_sub_4c4800 */

/* single caller course_warzone_cb0_sub_4c4830 */

undefined4 course_warzone_cb0_sub_4c4800(char *param_1)

{
  undefined4 uStack_4;
  
  uStack_4 = 0;
  RwFrameForAllChildren((struct RwFrame *)(*(RwFrame **)(param_1 + 4)),(RwFrameCallBack)gap_4c47c0,(void *)&uStack_4);
  return uStack_4;
}
