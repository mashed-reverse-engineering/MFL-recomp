#include "mfl_tier2.h"
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d49c0*/ extern RwMatrix * RwMatrixTransform(RwMatrix *p0, RwMatrix *p1, enum RwOpCombineType p2);

/* MATRIX_FLOATS: floats in a 4x4 RwMatrix. The ZONE_*_OFFSET values are byte offsets into the zone record (inferred from the bounds test below). */
#define MATRIX_FLOATS (0x10)
#define ZONE_MATRIX_OFFSET (0x54)
#define ZONE_MAX_X_OFFSET (0x14)
#define ZONE_MAX_Z_OFFSET (0x1c)
#define ZONE_MIN_X_OFFSET (0x20)
#define ZONE_MIN_Z_OFFSET (0x28)
#define ZONE_XFORM_OFFSET (0x94)

/* course_zone_contains_matrix */

/* "transforms a matrix into zone space and tests x/z against the zone bounds" */

/*
 * Transforms world_matrix into the zone's local space (using the inverse of the zone's
 * matrix at ZONE_MATRIX_OFFSET) and tests the resulting position against the zone's x/z bounds.
 * If inside, the zone-space matrix is further transformed by the matrix at ZONE_XFORM_OFFSET (inferred).
 * Parameters:
 *   zone         - zone record (char *, offsets are bytes)
 *   world_matrix - input matrix in world space, not modified
 *   out_values   - receives the 16 floats of the result matrix (the transformed one if inside, else world_matrix)
 * Returns 1 if the position is inside the zone bounds, 0 otherwise.
 */
undefined4 course_zone_contains_matrix(char *zone,RwMatrix *world_matrix,RwReal *out_values)

{
  undefined4 inside_zone;
  int floats_left;
  RwMatrix *src_cursor;
  RwMatrix *dst_cursor;
  RwMatrix zone_space_matrix;
  RwMatrix zone_inverse;

  /* Copy the 16 floats of world_matrix into zone_space_matrix. */
  src_cursor = world_matrix;
  dst_cursor = &zone_space_matrix;
  for (floats_left = MATRIX_FLOATS; floats_left != 0; floats_left = floats_left + -1) {
    (dst_cursor->right).x = (src_cursor->right).x;
    src_cursor = (RwMatrix *)&(src_cursor->right).y;
    dst_cursor = (RwMatrix *)&(dst_cursor->right).y;
  }
  /* Invert the zone's matrix, then move the world matrix into zone space with it. */
  RwMatrixInvert(&zone_inverse,(RwMatrix *)(zone + ZONE_MATRIX_OFFSET));
  RwMatrixTransform(&zone_space_matrix,&zone_inverse,rwCOMBINEPOSTCONCAT);
  /* Bounds test on the zone-space position: outside if x or z is at/past a bound. */
  /* NaN counts as outside (original: fcomp / test ah,5 / jp and fcomp / test ah,0x41 / jne) */
  if ((((!(zone_space_matrix.pos.x < *(float *)(zone + ZONE_MAX_X_OFFSET)) ||
       (!(zone_space_matrix.pos.x > *(float *)(zone + ZONE_MIN_X_OFFSET)))) ||
      (!(zone_space_matrix.pos.z < *(float *)(zone + ZONE_MAX_Z_OFFSET)))) ||
     (!(zone_space_matrix.pos.z > *(float *)(zone + ZONE_MIN_Z_OFFSET))))) {
    inside_zone = 0;
  }
  else {
    /* Inside: apply the zone transform (inferred) on top of the zone-space matrix. */
    RwMatrixTransform(&zone_space_matrix,*(RwMatrix **)(zone + ZONE_XFORM_OFFSET),rwCOMBINEPOSTCONCAT);
    world_matrix = &zone_space_matrix;
    inside_zone = 1;
  }
  /* Copy the 16 floats of the chosen matrix out to out_values. */
  for (floats_left = MATRIX_FLOATS; floats_left != 0; floats_left = floats_left + -1) {
    *out_values = (world_matrix->right).x;
    world_matrix = (RwMatrix *)&(world_matrix->right).y;
    out_values = out_values + 1;
  }
  return inside_zone;
}
