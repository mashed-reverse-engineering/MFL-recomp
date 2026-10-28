#include "mfl_tier2.h"
/*004d49c0*/ extern RwMatrix * RwMatrixTransform(RwMatrix *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d5640*/ extern struct RwV3d * RwV3dTransformPoint(struct RwV3d *p0, struct RwV3d *p1, RwMatrix *p2);
/*00556360*/ extern RwReal RpSplineFindMatrix(struct RpSpline *p0, RwInt32 p1, RwReal p2, struct RwV3d *p3, RwMatrix *p4);
/*007dc68c*/ extern char g_data_7dc68c[];
/* course_storm_cb1_sub_4ab160 */

/* single caller course_storm_cb1 */

/* Walks the spline state from t_start to t_end in steps of delta*t_step once (guarded by state[6]==0, then set to 0x40): samples the spline matrix, optionally transforms it by the matrix at state[7], and calls course_storm_cb1_sub_4aaea0 (inferred). */
void course_storm_cb1_sub_4ab160(undefined4 *state,float delta)

{
  RwV3d up_vector;
  RwMatrix spline_matrix;
  
  up_vector.x = 0.0;
  up_vector.y = 1.0;
  up_vector.z = 0.0;
  if (state[6] == 0) {
    state[6] = 0x40;
    state[4] = state[1];
    if (*(float *)&state[1] < *(float *)&state[2]) {
      do {
        RpSplineFindMatrix((RpSpline *)*state,10,*(float *)&state[4],&up_vector,&spline_matrix);
        if ((RwMatrix *)state[7] != (RwMatrix *)0x0) {
          RwMatrixTransform(&spline_matrix,(RwMatrix *)state[7],rwCOMBINEPOSTCONCAT);
          RwV3dTransformPoint((RwV3d *)(state + 8),(RwV3d *)(state + 0xc),(RwMatrix *)state[7]
                             );
        }
        course_storm_cb1_sub_4aaea0(&spline_matrix,state[5],((char *)&g_data_7dc68c + 0x54));
        *(float *)&state[4] = delta * *(float *)&state[3] + *(float *)&state[4];
      } while (*(float *)&state[4] < *(float *)&state[2]);
    }
  }
  return;
}


