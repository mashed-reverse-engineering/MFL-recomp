#include "mfl_tier2.h"
/*0048e720*/ extern undefined __cdecl rwp_object_set_matrix(int p0, RwMatrix *p1);
/*004c5360*/ extern void atomic_set_render_flag(RpAtomic * p0, int p1);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*006a2680*/ extern undefined4 g_course_objects_6a2680;
/*008c47d4*/ extern undefined4 g_course_objects_8c47d4;
/*008c4b1c*/ extern char g_course_objects_8c4b1c[];
#define RECORDS_PER_GROUP (10)
#define RECORD_STRIDE_INTS (7)
/* course_objects_reset_physics */
/* "clears the 10 object records per group and parks their physics objects" */
/* Resets the course object table. Parameters: none. Returns nothing.
 * The table is groups of RECORDS_PER_GROUP records, each RECORD_STRIDE_INTS ints:
 *   record[-5..-3]  position (RwV3d)            (inferred)
 *   record[-2]      RpAtomic to hide            (inferred)
 *   record[0]       physics body pointer, or 0  (inferred)
 *   record[1]       object handle for the matrix call (inferred)
 * Each record's physics body is detached and its matrix is set to its position.
 * Ends by clearing g_course_objects_6a2680. */
void course_objects_reset_physics(void)
{
  int object_handle;
  int records_left;
  int *record;
  RwMatrix matrix;

  /* walk every group until the end of the record table */
  record = &g_course_objects_8c47d4;
  do {
    /* one group: reset all of its records */
    records_left = RECORDS_PER_GROUP;
    do {
      object_handle = record[1];
      /* detach a live physics body: clear its state at +0xf4, then the pointer */
      if (*record != 0) {
        *(undefined4 *)(*record + 0xf4) = 0;
        *record = 0;
      }
      atomic_set_render_flag(record[-2],0);
      RwMatrixTranslate(&matrix,(RwV3d *)(record + -5),rwCOMBINEREPLACE);
      rwp_object_set_matrix(object_handle,&matrix);
      record = record + RECORD_STRIDE_INTS;
      records_left = records_left + -1;
    } while (records_left != 0);
  } while ((LONG_PTR)record < ((LONG_PTR)&g_course_objects_8c4b1c));
  g_course_objects_6a2680 = 0;
  return;
}
