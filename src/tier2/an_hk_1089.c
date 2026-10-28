#include "mfl_tier2.h"
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/* course_nt1_helper_4557f0_sub_4a5ba0 */

/* single caller course_nt1_helper_4557f0 */

/* For each of the frames[0x62] frames: rotate by its angle (read at angles_base + slot offset) about axis while keeping its current position (inferred). */
void course_nt1_helper_4557f0_sub_4a5ba0(undefined4 *frames,RwV3d *axis,int angles_base)

{
  RwFrame *frame;
  undefined4 *frame_slot;
  int index;
  RwV3d saved_pos;
  
  index = 0;
  if (0 < (int)frames[0x62]) {
    frame_slot = frames;
    do {
      frame = (RwFrame *)*frame_slot;
      saved_pos.x = RwFrame_modelling(frame).pos.x;
      saved_pos.y = RwFrame_modelling(frame).pos.y;
      saved_pos.z = RwFrame_modelling(frame).pos.z;
      RwFrameRotate(frame,axis,*(RwReal *)((char *)frame_slot + (angles_base - (int)frames)),
                    rwCOMBINEREPLACE);
      RwFrameTranslate((RwFrame *)*frame_slot,&saved_pos,rwCOMBINEPOSTCONCAT);
      index = index + 1;
      frame_slot = frame_slot + 1;
    } while (index < (int)frames[0x62]);
  }
  return;
}


