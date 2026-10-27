#include "mfl_tier2.h"
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd364*/ extern float k_f_n1;
/* course_storm_cb1_helper_485250 */

/* callers course_storm_cb1 */

void course_storm_cb1_helper_485250(RwMatrix *param_1,RwV3d *param_2,RwReal param_3)

{
  RwV3d RStack_c;
  
  RStack_c.x = (param_1->pos).x * k_f_n1;
  RStack_c.y = (param_1->pos).y * k_f_n1;
  RStack_c.z = (param_1->pos).z * k_f_n1;
  RwMatrixTranslate(param_1,&RStack_c,rwCOMBINEPOSTCONCAT);
  RwMatrixRotate(param_1,param_2,param_3,rwCOMBINEPOSTCONCAT);
  RStack_c.x = RStack_c.x * k_f_n1;
  RStack_c.y = RStack_c.y * k_f_n1;
  RStack_c.z = RStack_c.z * k_f_n1;
  RwMatrixTranslate(param_1,&RStack_c,rwCOMBINEPOSTCONCAT);
  return;
}


