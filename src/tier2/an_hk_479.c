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
 * camera: camera object, its RwFrame * sits at +0x4 (inferred).
 */

void camera_view_matrix_from_frame(float *view_out,char *camera)

{
  RwMatrix *frame_ltm;
  int remaining;
  float *src;
  struct {
    RwMatrix local_80;
    float local_40 [4];
    RwReal local_30;
    RwReal local_2c;
    RwReal local_28;
    RwReal local_24;
    RwReal local_20;
    RwReal local_1c;
    RwReal local_18;
    RwReal local_14;
    undefined4 local_10;
    undefined4 local_c;
    undefined4 local_8;
    undefined4 local_4;
  } scratch;

  /* frame_ltm: the frame's local-to-world matrix; the inverse lands in scratch.local_80 */
  frame_ltm = RwFrameGetLTM(*(RwFrame **)(camera + 4));
  /* start scratch.local_80 from identity (unit right/up/at, zero pos), then invert the LTM into it */
  scratch.local_80.flags = scratch.local_80.flags | MATRIX_FLAGS_IDENTITY_ORTHONORMAL;
  scratch.local_80.at.z = 1.0;
  scratch.local_80.up.y = 1.0;
  scratch.local_80.right.x = 1.0;
  scratch.local_80.up.x = 0.0;
  scratch.local_80.right.z = 0.0;
  scratch.local_80.right.y = 0.0;
  scratch.local_80.at.y = 0.0;
  scratch.local_80.at.x = 0.0;
  scratch.local_80.up.z = 0.0;
  scratch.local_80.pos.z = 0.0;
  scratch.local_80.pos.y = 0.0;
  scratch.local_80.pos.x = 0.0;
  RwMatrixInvert(&scratch.local_80,frame_ltm);
  scratch.local_40[0] = -scratch.local_80.right.x;
  scratch.local_40[1] = -scratch.local_80.up.x;
  scratch.local_2c = scratch.local_80.up.y;
  scratch.local_30 = scratch.local_80.right.y;
  scratch.local_40[2] = -scratch.local_80.at.x;
  scratch.local_20 = scratch.local_80.right.z;
  scratch.local_28 = scratch.local_80.at.y;
  scratch.local_40[3] = -scratch.local_80.pos.x;
  scratch.local_24 = scratch.local_80.pos.y;
  scratch.local_14 = scratch.local_80.pos.z;
  scratch.local_1c = scratch.local_80.up.z;
  scratch.local_18 = scratch.local_80.at.z;
  /* last 4 of the 16 floats: 0, 0, 0, then 1.0f (FLOAT_ONE_BITS) */
  scratch.local_10 = 0;
  scratch.local_c = 0;
  scratch.local_8 = 0;
  scratch.local_4 = FLOAT_ONE_BITS;
  /* copy the 16 staged floats to view_out, counting down */
  src = scratch.local_40;
  for (remaining = VIEW_MATRIX_FLOAT_COUNT; remaining != 0; remaining = remaining + -1) {
    *view_out = *src;
    src = src + 1;
    view_out = view_out + 1;
  }
  return;
}


