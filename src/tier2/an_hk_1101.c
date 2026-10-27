#include "mfl_tier2.h"
/*004d49c0*/ extern RwMatrix * RwMatrixTransform(RwMatrix *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d5640*/ extern struct RwV3d * RwV3dTransformPoint(struct RwV3d *p0, struct RwV3d *p1, RwMatrix *p2);
/*00556360*/ extern RwReal RpSplineFindMatrix(struct RpSpline *p0, RwInt32 p1, RwReal p2, struct RwV3d *p3, RwMatrix *p4);
/*007dc68c*/ extern char g_data_7dc68c[];
/* course_storm_cb1_sub_4ab160 */

/* single caller course_storm_cb1 */

void course_storm_cb1_sub_4ab160(undefined4 *param_1,float param_2)

{
  RwV3d RStack_4c;
  RwMatrix RStack_40;
  
  RStack_4c.x = 0.0;
  RStack_4c.y = 1.0;
  RStack_4c.z = 0.0;
  if (param_1[6] == 0) {
    param_1[6] = 0x40;
    param_1[4] = param_1[1];
    if (*(float *)&param_1[1] < *(float *)&param_1[2]) {
      do {
        RpSplineFindMatrix((RpSpline *)*param_1,10,*(float *)&param_1[4],&RStack_4c,&RStack_40);
        if ((RwMatrix *)param_1[7] != (RwMatrix *)0x0) {
          RwMatrixTransform(&RStack_40,(RwMatrix *)param_1[7],rwCOMBINEPOSTCONCAT);
          RwV3dTransformPoint((RwV3d *)(param_1 + 8),(RwV3d *)(param_1 + 0xc),(RwMatrix *)param_1[7]
                             );
        }
        course_storm_cb1_sub_4aaea0(&RStack_40,param_1[5],((char *)&g_data_7dc68c + 0x54));
        *(float *)&param_1[4] = param_2 * *(float *)&param_1[3] + *(float *)&param_1[4];
      } while (*(float *)&param_1[4] < *(float *)&param_1[2]);
    }
  }
  return;
}


