#include "mfl_tier2.h"

/* camera struct offsets: the role of each is inferred from how the values are used (see the function comment) */
#define CAMERA_SCALE_X_OFFSET (0x70)
#define CAMERA_SCALE_Y_OFFSET (0x74)
#define CAMERA_NEAR_OFFSET (0x80)
#define CAMERA_FAR_OFFSET (0x84)
/* 1.0f as raw bits */
#define FLOAT_ONE_BITS (0x3f800000)

/* camera_view_params_pack */

/* Writes a 4x4 perspective projection matrix (16 dwords) into out_matrix from the camera (inferred).
 *   out_matrix[0]  = camera +0x70 (x scale, inferred)
 *   out_matrix[5]  = camera +0x74 (y scale, inferred)
 *   out_matrix[10] = depth_scale = far / (far - near), far = camera +0x84, near = camera +0x80 (inferred)
 *   out_matrix[11] = -(depth_scale * near) (inferred)
 *   out_matrix[14] = 1.0f; every other dword is 0.
 * camera is the camera pointer (not a struct pointer in mfl_tier2.h yet). param_3..param_6 are unused.
 * (was: "packs the +0x70/+0x74 fields of a camera into an 8 dword structure (zeroing the rest)") */

void __cdecl camera_view_params_pack(undefined4 *out_matrix,char *camera,undefined4 param_3,undefined4 unused_4,undefined4 unused_5,
          undefined4 param_6)

{
  float depth_scale;
  float near_plane;

  *out_matrix = *(undefined4 *)(camera + CAMERA_SCALE_X_OFFSET);
  out_matrix[1] = 0;
  out_matrix[2] = 0;
  out_matrix[3] = 0;
  out_matrix[4] = 0;
  out_matrix[5] = *(undefined4 *)(camera + CAMERA_SCALE_Y_OFFSET);
  out_matrix[6] = 0;
  out_matrix[7] = 0;
  out_matrix[8] = 0;
  out_matrix[9] = 0;
  depth_scale = *(float *)(camera + CAMERA_FAR_OFFSET) / (*(float *)(camera + CAMERA_FAR_OFFSET) - *(float *)(camera + CAMERA_NEAR_OFFSET));
  *(float *)&out_matrix[10] = depth_scale;
  near_plane = *(float *)(camera + CAMERA_NEAR_OFFSET);
  out_matrix[0xc] = 0;
  out_matrix[0xd] = 0;
  out_matrix[0xe] = FLOAT_ONE_BITS;
  out_matrix[0xf] = 0;
  *(float *)&out_matrix[0xb] = -(depth_scale * near_plane);
  return;
}


