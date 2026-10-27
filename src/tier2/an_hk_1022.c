#include "mfl_tier2.h"
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d56a0*/ extern struct RwV3d * RwV3dTransformVector(struct RwV3d *p0, struct RwV3d *p1, RwMatrix *p2);
/*005de1fc*/ extern float k_f_111;
/*00658298*/ extern float g_data_658298;
/*0065829c*/ extern float g_data_65829c;
/*006582a0*/ extern float g_data_6582a0;
/*006582a4*/ extern float g_data_6582a4;
/*006582a8*/ extern float g_data_6582a8;
/*006582ac*/ extern float g_data_6582ac;
/*006582b0*/ extern float g_data_6582b0;
/*008c9ee0*/ extern float g_noise_table_8c9ee0;
/*008c9ee4*/ extern float g_noise_table_8c9ee4;
/*008c9ee8*/ extern float g_noise_table_8c9ee8;
/*008c9eec*/ extern float g_noise_table_8c9eec;
/*00416840*/ extern float course_helper_449c40_sub_416840(float p0, int p1);
/* course_helper_449c40_sub_416870 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* single caller course_helper_449c40 */

void course_helper_449c40_sub_416870(float param_1)

{
  double fVar1;
  RwV3d RStack_4c;
  RwMatrix RStack_40;
  
  RStack_40.flags = RStack_40.flags | 0x20003;
  RStack_4c.x = 0.0;
  RStack_4c.y = 1.0;
  RStack_4c.z = 0.0;
  RStack_40.at.z = 1.0;
  RStack_40.up.y = 1.0;
  RStack_40.right.x = 1.0;
  RStack_40.up.x = 0.0;
  RStack_40.right.z = 0.0;
  RStack_40.right.y = 0.0;
  RStack_40.at.y = 0.0;
  RStack_40.at.x = 0.0;
  RStack_40.up.z = 0.0;
  RStack_40.pos.z = 0.0;
  RStack_40.pos.y = 0.0;
  RStack_40.pos.x = 0.0;
  g_data_658298 = 0.0;
  g_data_65829c = 0.0;
  g_data_6582a0 = 1.0;
  fVar1 = (double)course_helper_449c40_sub_416840(g_data_6582a4,0);
  g_data_6582ac = (float)(fVar1 * (double)g_noise_table_8c9ee8);
  fVar1 = (double)course_helper_449c40_sub_416840(g_data_6582a8 + k_f_111,0);
  g_data_6582b0 = (float)(fVar1 * (double)g_noise_table_8c9eec);
  g_data_6582a4 = g_noise_table_8c9ee0 * param_1 + g_data_6582a4;
  g_data_6582a8 = g_noise_table_8c9ee4 * param_1 + g_data_6582a8;
  RwMatrixRotate(&RStack_40,&RStack_4c,g_data_6582ac,rwCOMBINEREPLACE);
  RwV3dTransformVector((RwV3d *)&g_data_658298,(RwV3d *)&g_data_658298,&RStack_40);
  g_data_658298 = g_data_6582b0 * g_data_658298;
  g_data_65829c = g_data_6582b0 * g_data_65829c;
  g_data_6582a0 = g_data_6582b0 * g_data_6582a0;
  return;
}


