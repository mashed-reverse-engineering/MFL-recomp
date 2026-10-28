#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*004c45c0*/ extern int rp_random_int_range(int p0, int p1);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);

/* IEEE-754 bit patterns of the float values 1.0f and 360.0f */
#define FLOAT_360_BITS (0x43b40000)

/* course_arctic_cb3_object_init */

/* "initialises one rotating object (matrix and scales)" */

/*
 * Sets up one rotating course object: writes the default scale values, an
 * RwMatrix that sits after the object's first int (translated to z = 10, then
 * rotated about the Y axis by a random angle in [0, 360) degrees), and two
 * random 0/1 flags (meaning inferred). Returns nothing.
 *
 * object: int record; object[0] is the init flag (0 = not yet initialised).
 */
void __cdecl course_arctic_cb3_object_init(int *object)

{
  RwMatrix *matrix;
  int random_flag;
  double angle;
  RwOpCombineType combineOp;
  RwV3d translation;
  RwV3d rotation_axis;

  translation.x = 0.0;
  translation.y = 0.0;
  translation.z = 10.0;
  rotation_axis.x = 0.0;
  rotation_axis.y = 1.0;
  rotation_axis.z = 0.0;
  /* only initialise once: the flag is set to 1 at the end of this block */
  if (*object == 0) {
    /* default scale slots (inferred) */
    object[0xb] = FLOAT_ONE_BITS;
    object[6] = FLOAT_ONE_BITS;
    matrix = (RwMatrix *)(object + 1);
    (matrix->right).x = 1.0;
    /* flags word: set bits 0x20003 (meaning inferred) */
    object[4] = object[4] | 0x20003;
    /* clear the remaining state words */
    object[5] = 0;
    object[3] = 0;
    object[2] = 0;
    object[10] = 0;
    object[9] = 0;
    object[7] = 0;
    object[0xf] = 0;
    object[0xe] = 0;
    object[0xd] = 0;
    /* place the matrix: translate to (0, 0, 10), then rotate about Y */
    RwMatrixTranslate(matrix,&translation,rwCOMBINEREPLACE);
    combineOp = rwCOMBINEPOSTCONCAT;
    angle = (double)random_range(0,FLOAT_360_BITS);
    RwMatrixRotate(matrix,&rotation_axis,(float)angle,combineOp);
    /* two random 0/1 flags, stored after the matrix (meaning inferred) */
    random_flag = rp_random_int_range(0,1);
    object[0x11] = random_flag;
    random_flag = rp_random_int_range(0,1);
    object[0x12] = random_flag;
    *object = 1;
  }
  return;
}
