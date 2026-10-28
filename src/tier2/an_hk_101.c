#include "mfl_tier2.h"
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d3cd0*/ extern RwMatrix * RwMatrixMultiply(RwMatrix *p0, RwMatrix *p1, RwMatrix *p2);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
#define RW_MATRIX_WORDS (0x10)
/* rwframe_look_at */

/*
 * Re-aims a frame so its local +Z ("at") axis points at target_pos, keeping the
 * frame's world position (inferred from the code). Builds an orthonormal look-at
 * basis from the position to target_pos, with world_up as the roll hint, then
 * applies the change (look-at * inverse(current LTM)) to the frame with
 * RwFrameTransform in PRECONCAT mode.
 *   frame      - frame to re-aim; its LTM is read and then modified
 *   target_pos - world-space point to look at (float[3])
 *   world_up   - up hint, float[3] (inferred)
 * Returns nothing.
 */
void rwframe_look_at(RwFrame *frame,float *target_pos,float *world_up)

{
  RwMatrix *src;
  int words_left;
  RwMatrix *dst;
  RwMatrix look_at_ltm;
  RwMatrix ltm_inv;
  RwMatrix delta;

  src = RwFrameGetLTM(frame);
  look_at_ltm.pos.x = (src->pos).x;
  look_at_ltm.pos.y = (src->pos).y;
  look_at_ltm.pos.z = (src->pos).z;
  look_at_ltm.at.x = *target_pos - look_at_ltm.pos.x;
  look_at_ltm.at.y = target_pos[1] - look_at_ltm.pos.y;
  look_at_ltm.at.z = target_pos[2] - look_at_ltm.pos.z;
  RwV3dNormalize(&look_at_ltm.at,&look_at_ltm.at);
  look_at_ltm.right.x = look_at_ltm.at.z * world_up[1] - look_at_ltm.at.y * world_up[2];
  look_at_ltm.right.y = look_at_ltm.at.x * world_up[2] - look_at_ltm.at.z * *world_up;
  look_at_ltm.right.z = look_at_ltm.at.y * *world_up - look_at_ltm.at.x * world_up[1];
  RwV3dNormalize(&look_at_ltm.right,&look_at_ltm.right);
  look_at_ltm.up.x = look_at_ltm.right.z * look_at_ltm.at.y - look_at_ltm.right.y * look_at_ltm.at.z;
  look_at_ltm.up.y = look_at_ltm.right.x * look_at_ltm.at.z - look_at_ltm.right.z * look_at_ltm.at.x;
  look_at_ltm.up.z = look_at_ltm.right.y * look_at_ltm.at.x - look_at_ltm.right.x * look_at_ltm.at.y;
  RwV3dNormalize(&look_at_ltm.up,&look_at_ltm.up);
  RwMatrixUpdate(&look_at_ltm);
  RwMatrixInvert(&ltm_inv,src);
  RwMatrixMultiply(&delta,&look_at_ltm,&ltm_inv);
  /* copy the 16-word delta over look_at_ltm, word by word (src walks delta, dst walks look_at_ltm) */
  src = &delta;
  dst = &look_at_ltm;
  for (words_left = RW_MATRIX_WORDS;words_left != 0; words_left = words_left + -1) {
    (dst->right).x = (src->right).x;
    src = (RwMatrix *)&(src->right).y;
    dst = (RwMatrix *)&(dst->right).y;
  }
  /* apply the delta to the frame (look_at_ltm now holds the delta) */
  RwFrameTransform(frame,&look_at_ltm,rwCOMBINEPRECONCAT);
  return;
}


