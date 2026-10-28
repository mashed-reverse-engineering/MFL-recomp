#include "mfl_tier2.h"
/*004d3d40*/ extern int MatrixOrthoNormalize_004d3d50();
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d49c0*/ extern RwMatrix * RwMatrixTransform(RwMatrix *p0, RwMatrix *p1, enum RwOpCombineType p2);
/* course_warzone_cb1_sub_45cd70 */

/* single caller course_warzone_cb1 */

/* Copies the 16 floats src_floats into a matrix, post-concats inverse(obj+0x54) and *(obj+0x94), orthonormalizes it and copies the 16 floats to dst_floats; returns 1 (inferred). */
undefined4 course_warzone_cb1_sub_45cd70(char *obj,RwReal *src_floats,RwReal *dst_floats)

{
  int remaining;
  RwMatrix *mtx_cursor;
  RwMatrix work_mtx;
  RwMatrix inv_mtx;
  
  mtx_cursor = &work_mtx;
  for (remaining = 0x10; remaining != 0; remaining = remaining + -1) {
    (mtx_cursor->right).x = *src_floats;
    src_floats = src_floats + 1;
    mtx_cursor = (RwMatrix *)&(mtx_cursor->right).y;
  }
  RwMatrixInvert(&inv_mtx,(RwMatrix *)(obj + 0x54));
  RwMatrixTransform(&work_mtx,&inv_mtx,rwCOMBINEPOSTCONCAT);
  RwMatrixTransform(&work_mtx,*(RwMatrix **)(obj + 0x94),rwCOMBINEPOSTCONCAT);
  MatrixOrthoNormalize_004d3d50(&work_mtx,&work_mtx);
  mtx_cursor = &work_mtx;
  for (remaining = 0x10; remaining != 0; remaining = remaining + -1) {
    *dst_floats = (mtx_cursor->right).x;
    mtx_cursor = (RwMatrix *)&(mtx_cursor->right).y;
    dst_floats = dst_floats + 1;
  }
  return 1;
}


