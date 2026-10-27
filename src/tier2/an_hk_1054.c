#include "mfl_tier2.h"
/*004d0f70*/ extern struct RwFrame * RwFrameUpdateObjects(struct RwFrame *p0);
/*004d1160*/ extern struct RwFrame * RwFrameAddChild(struct RwFrame *p0, struct RwFrame *p1);
/*004d1330*/ extern struct RwFrame * RwFrameRemoveChild(struct RwFrame *p0);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd410*/ extern float k_f_n2;
/* course_superg_cb4_sub_45c140 */

/* single caller course_superg_cb4_sub_45c1f0 */

void course_superg_cb4_sub_45c140(char *param_1,RwMatrix *param_2)

{
  RwMatrix *matrix;
  RwFrame *child;
  RwFrame *parent;
  int iVar1;
  RwMatrix *pRVar2;
  RwMatrix *pRVar3;
  RwV3d RStack_4c;
  RwMatrix RStack_40;
  
  child = *(RwFrame **)(param_1 + 4);
  matrix = &RwFrame_modelling(child);
  RStack_4c.x = 0.0;
  RStack_4c.y = 0.0;
  RStack_4c.z = 0.0;
  pRVar2 = matrix;
  pRVar3 = &RStack_40;
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    (pRVar3->right).x = (pRVar2->right).x;
    pRVar2 = (RwMatrix *)&(pRVar2->right).y;
    pRVar3 = (RwMatrix *)&(pRVar3->right).y;
  }
  parent = RwFrame_object_parent(child);
  RwFrameRemoveChild(child);
  RwFrameTransform(child,param_2,rwCOMBINEREPLACE);
  RStack_4c.y = (param_2->pos).y * k_f_n2;
  course_storm_cb1_helper_485250(matrix,&param_2->at,0x43340000);
  RwMatrixTranslate(matrix,&RStack_4c,rwCOMBINEPOSTCONCAT);
  RwFrameUpdateObjects(child);
  (**(code **)(param_1 + 0x48))(param_1);
  RwFrameTransform(child,&RStack_40,rwCOMBINEREPLACE);
  RwFrameAddChild(parent,child);
  return;
}


