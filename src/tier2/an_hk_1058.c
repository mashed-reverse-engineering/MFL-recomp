#include "mfl_tier2.h"
/*004d3d40*/ extern int MatrixOrthoNormalize_004d3d50();
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d49c0*/ extern RwMatrix * RwMatrixTransform(RwMatrix *p0, RwMatrix *p1, enum RwOpCombineType p2);
/* course_warzone_cb1_sub_45cd70 */

/* single caller course_warzone_cb1 */

undefined4 course_warzone_cb1_sub_45cd70(char *param_1,RwReal *param_2,RwReal *param_3)

{
  int iVar1;
  RwMatrix *pRVar2;
  RwMatrix RStack_80;
  RwMatrix RStack_40;
  
  pRVar2 = &RStack_80;
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    (pRVar2->right).x = *param_2;
    param_2 = param_2 + 1;
    pRVar2 = (RwMatrix *)&(pRVar2->right).y;
  }
  RwMatrixInvert(&RStack_40,(RwMatrix *)(param_1 + 0x54));
  RwMatrixTransform(&RStack_80,&RStack_40,rwCOMBINEPOSTCONCAT);
  RwMatrixTransform(&RStack_80,*(RwMatrix **)(param_1 + 0x94),rwCOMBINEPOSTCONCAT);
  MatrixOrthoNormalize_004d3d50(&RStack_80,&RStack_80);
  pRVar2 = &RStack_80;
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    *param_3 = (pRVar2->right).x;
    pRVar2 = (RwMatrix *)&(pRVar2->right).y;
    param_3 = param_3 + 1;
  }
  return 1;
}


