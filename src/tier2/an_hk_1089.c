#include "mfl_tier2.h"
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/* course_nt1_helper_4557f0_sub_4a5ba0 */

/* single caller course_nt1_helper_4557f0 */

void course_nt1_helper_4557f0_sub_4a5ba0(undefined4 *param_1,RwV3d *param_2,int param_3)

{
  RwFrame *frame;
  undefined4 *puVar1;
  int iVar2;
  RwV3d RStack_c;
  
  iVar2 = 0;
  if (0 < (int)param_1[0x62]) {
    puVar1 = param_1;
    do {
      frame = (RwFrame *)*puVar1;
      RStack_c.x = RwFrame_modelling(frame).pos.x;
      RStack_c.y = RwFrame_modelling(frame).pos.y;
      RStack_c.z = RwFrame_modelling(frame).pos.z;
      RwFrameRotate(frame,param_2,*(RwReal *)((char *)puVar1 + (param_3 - (int)param_1)),
                    rwCOMBINEREPLACE);
      RwFrameTranslate((RwFrame *)*puVar1,&RStack_c,rwCOMBINEPOSTCONCAT);
      iVar2 = iVar2 + 1;
      puVar1 = puVar1 + 1;
    } while (iVar2 < (int)param_1[0x62]);
  }
  return;
}


