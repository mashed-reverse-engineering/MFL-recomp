#include "mfl_tier2.h"


/* matrix_transpose_row_copy: copies the 4 elements of a matrix column into a row vector */

void matrix_transpose_row_copy(undefined4 *dst_matrix,undefined4 *src_matrix)
{
  /* dst (4x4, row-major) = transpose of src's 3x4 part (src[0..2] rows, column 3 = translation); dst row 3 is (0,0,0,1) */
  dst_matrix[0] = src_matrix[0];
  dst_matrix[1] = src_matrix[4];
  dst_matrix[2] = src_matrix[8];
  dst_matrix[3] = src_matrix[0xc];
  dst_matrix[4] = src_matrix[1];
  dst_matrix[5] = src_matrix[5];
  dst_matrix[6] = src_matrix[9];
  dst_matrix[7] = src_matrix[0xd];
  dst_matrix[8] = src_matrix[2];
  dst_matrix[9] = src_matrix[6];
  dst_matrix[10] = src_matrix[10];
  dst_matrix[0xb] = src_matrix[0xe];
  dst_matrix[0xc] = 0;
  dst_matrix[0xd] = 0;
  dst_matrix[0xe] = 0;
  dst_matrix[0xf] = 0x3f800000;   /* 1.0f */
  return;
}
