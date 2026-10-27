#include "mfl_tier2.h"
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
#define MATRIX_FLAGS_IDENTITY_ORTHONORMAL (0x20003)
#define VIEW_MATRIX_FLOAT_COUNT (16)
#define FLOAT_ONE_BITS (0x3f800000)
/* camera_view_matrix_from_frame */

/* "inverts the frame LTM and writes the 4x4 view matrix (x/y/z axis negated)"
 * Builds the camera view matrix from the camera's frame: inverts the frame LTM
 * (frame pointer at camera+0x4) into a scratch matrix, then stages the 16 floats
 * with the x/y/z axes negated (inferred from the code) and copies them out.
 * view_out: 16 floats written, destination (not returned).

 * (inferred) the rows of the staged matrix are the inverse rotation columns: only row 0 (x) is negated, rows 1 and 2 are copied as they are.
 * camera: camera object, its RwFrame * sits at +0x4 (inferred).
 */

void camera_view_matrix_from_frame(float *view_out,char *camera)

{
  RwMatrix *frame_ltm;
  int remaining;
  float *src;
  struct {
    RwMatrix inv_ltm;
    float view_r0 [4];
    RwReal view_r1_c0;
    RwReal view_r1_c1;
    RwReal view_r1_c2;
    RwReal view_r1_c3;
    RwReal view_r2_c0;
    RwReal view_r2_c1;
    RwReal view_r2_c2;
    RwReal view_r2_c3;
    undefined4 view_r3_c0;
    undefined4 view_r3_c1;
    undefined4 view_r3_c2;
    undefined4 view_r3_c3;
  } scratch;

  /* frame_ltm: the frame's local-to-world matrix; the inverse lands in scratch.inv_ltm */
  frame_ltm = RwFrameGetLTM(*(RwFrame **)(camera + 4));
  /* start scratch.inv_ltm from identity (unit right/up/at, zero pos), then invert the LTM into it */
  scratch.inv_ltm.flags = scratch.inv_ltm.flags | MATRIX_FLAGS_IDENTITY_ORTHONORMAL;
  scratch.inv_ltm.at.z = 1.0;
  scratch.inv_ltm.up.y = 1.0;
  scratch.inv_ltm.right.x = 1.0;
  scratch.inv_ltm.up.x = 0.0;
  scratch.inv_ltm.right.z = 0.0;
  scratch.inv_ltm.right.y = 0.0;
  scratch.inv_ltm.at.y = 0.0;
  scratch.inv_ltm.at.x = 0.0;
  scratch.inv_ltm.up.z = 0.0;
  scratch.inv_ltm.pos.z = 0.0;
  scratch.inv_ltm.pos.y = 0.0;
  scratch.inv_ltm.pos.x = 0.0;
  RwMatrixInvert(&scratch.inv_ltm,frame_ltm);
  scratch.view_r0[0] = -scratch.inv_ltm.right.x;
  scratch.view_r0[1] = -scratch.inv_ltm.up.x;
  scratch.view_r1_c1 = scratch.inv_ltm.up.y;
  scratch.view_r1_c0 = scratch.inv_ltm.right.y;
  scratch.view_r0[2] = -scratch.inv_ltm.at.x;
  scratch.view_r2_c0 = scratch.inv_ltm.right.z;
  scratch.view_r1_c2 = scratch.inv_ltm.at.y;
  scratch.view_r0[3] = -scratch.inv_ltm.pos.x;
  scratch.view_r1_c3 = scratch.inv_ltm.pos.y;
  scratch.view_r2_c3 = scratch.inv_ltm.pos.z;
  scratch.view_r2_c1 = scratch.inv_ltm.up.z;
  scratch.view_r2_c2 = scratch.inv_ltm.at.z;
  /* last 4 of the 16 floats: 0, 0, 0, then 1.0f (FLOAT_ONE_BITS) */
  scratch.view_r3_c0 = 0;
  scratch.view_r3_c1 = 0;
  scratch.view_r3_c2 = 0;
  scratch.view_r3_c3 = FLOAT_ONE_BITS;
  /* copy the 16 staged floats to view_out, counting down */
  src = scratch.view_r0;
  for (remaining = VIEW_MATRIX_FLOAT_COUNT; remaining != 0; remaining = remaining + -1) {
    *view_out = *src;
    src = src + 1;
    view_out = view_out + 1;
  }
  return;
}


