#include "mfl_tier2.h"
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/* FUN_004c4460 */
/* Rebuilds an orientation matrix so its at vector points from the matrix
 * position towards target_pos (inferred). right = normalize(cross(up_hint, at)),
 * up = normalize(cross(at, right)), then RwMatrixUpdate(matrix).
 * matrix is laid out as RwMatrix viewed through RwV3d: right at +0x0,
 * up at +0x10, at at +0x20, pos at +0x30 (inferred). Returns nothing. */

void FUN_004c4460(RwV3d *matrix,float *target_pos,float *up_hint)

{
  RwReal *at_vec;
  RwReal *up_vec;

  /* at = target_pos - pos, then normalized in place */
  at_vec = &matrix[2].z;
  *at_vec = *target_pos - matrix[4].x;
  up_vec = &matrix[1].y;
  matrix[3].x = target_pos[1] - matrix[4].y;
  matrix[3].y = target_pos[2] - matrix[4].z;
  RwV3dNormalize((RwV3d *)at_vec,(RwV3d *)at_vec);
  /* right = normalize(cross(up_hint, at)) */
  matrix->x = matrix[3].y * up_hint[1] - up_hint[2] * matrix[3].x;
  matrix->y = *at_vec * up_hint[2] - *up_hint * matrix[3].y;
  matrix->z = *up_hint * matrix[3].x - *at_vec * up_hint[1];
  RwV3dNormalize(matrix,matrix);
  /* up = normalize(cross(at, right)) */
  *up_vec = matrix->z * matrix[3].x - matrix->y * matrix[3].y;
  matrix[1].z = matrix->x * matrix[3].y - *at_vec * matrix->z;
  matrix[2].x = *at_vec * matrix->y - matrix->x * matrix[3].x;
  RwV3dNormalize((RwV3d *)up_vec,(RwV3d *)up_vec);
  RwMatrixUpdate(matrix);
  return;
}


