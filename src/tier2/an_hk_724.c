#include "mfl_tier2.h"
/*0048a060*/ extern int spring_grid_reset_constants();
/*0048a0c0*/ extern int spring_grid_build();
/*004d4e70*/ extern RwMatrix * RwMatrixCreate();
/*006f2ab4*/ extern RwMatrix *g_spring_grid_6f2ab4;

/* rwMATRIXINTERNALIDENTITY (0x20000) | rwMATRIXTYPEORTHONORMAL (3), see RwMatrixFlag in ghidra_types.h */
#define RW_MATRIX_FLAGS_IDENTITY_ORTHONORMAL (0x20003)

/* spring_grid_init
 * Resets the spring grid constants, creates the grid's RwMatrix in g_spring_grid_6f2ab4,
 * sets that matrix to the identity (right = X, up = Y, at = Z, pos = origin), marks it
 * with the identity and orthonormal flags, then builds the grid.
 * No parameters, no return value.
 * (was: "creates the matrix and resets the spring grid constants (23 x 23 point grid, inferred)";
 * the 23 x 23 grid size is not set in this function, so it is not kept here)
 */

void spring_grid_init(void)

{
  spring_grid_reset_constants();
  g_spring_grid_6f2ab4 = RwMatrixCreate();

  /* identity matrix: basis vectors right = X, up = Y, at = Z, and pos = origin */
  (g_spring_grid_6f2ab4->at).z = 1.0;
  (g_spring_grid_6f2ab4->up).y = 1.0;
  (g_spring_grid_6f2ab4->right).x = 1.0;
  (g_spring_grid_6f2ab4->up).x = 0.0;
  (g_spring_grid_6f2ab4->right).z = 0.0;
  (g_spring_grid_6f2ab4->right).y = 0.0;
  (g_spring_grid_6f2ab4->at).y = 0.0;
  (g_spring_grid_6f2ab4->at).x = 0.0;
  (g_spring_grid_6f2ab4->up).z = 0.0;
  (g_spring_grid_6f2ab4->pos).z = 0.0;
  (g_spring_grid_6f2ab4->pos).y = 0.0;
  (g_spring_grid_6f2ab4->pos).x = 0.0;
  g_spring_grid_6f2ab4->flags = g_spring_grid_6f2ab4->flags | RW_MATRIX_FLAGS_IDENTITY_ORTHONORMAL;
  spring_grid_build();
  return;
}


