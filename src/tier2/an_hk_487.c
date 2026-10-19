#include "mfl_tier2.h"
/*00495c30*/ extern int viewport_get_camera();
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
#define FRAME_SLOT_COUNT (4)
#define SLOT_TABLE_OFFSET (0xc0f0)
#define FLIP_ANGLE_DEG (180.0)
#define CAMERA_POS_OFFSET (0x40)
/* course_arctic_camera_relative_update */

/*
 * Sets the RwFrame of each occupied slot (4 slots) relative to the viewport camera: the frame is reset,
 * placed at the origin, flipped 180 deg about the x axis, then 180 deg about the y axis, and finally
 * translated by the camera position (camera +0x40, inferred).
 * state_base: game state block (inferred); its slot table at +0xc0f0 holds pointers to slot objects,
 * whose +4 is the RwFrame (inferred).
 * Returns nothing.
 */

void course_arctic_camera_relative_update(char *state_base)

{
  RwFrame *frame;
  char *viewport_camera;
  int *slot;
  int slots_left;
  struct {
    RwV3d origin;
    RwV3d x_axis;
    RwV3d y_axis;
  } frame_obj;

  viewport_camera = viewport_get_camera(0);
  viewport_camera = *(char **)(viewport_camera + 4);
  frame_obj.origin.x = 0.0;
  frame_obj.origin.y = 0.0;
  frame_obj.origin.z = 0.0;
  frame_obj.x_axis.x = 1.0;
  frame_obj.x_axis.y = 0.0;
  frame_obj.x_axis.z = 0.0;
  frame_obj.y_axis.x = 0.0;
  frame_obj.y_axis.y = 1.0;
  frame_obj.y_axis.z = 0.0;
  slot = (int *)(state_base + SLOT_TABLE_OFFSET);
  slots_left = FRAME_SLOT_COUNT;
  /* walk the slot table, one entry per iteration */
  do {
    /* skip empty slots */
    if (*slot != 0) {
      frame = *(RwFrame **)(*slot + 4);
      RwFrameSetIdentity(frame);
      RwFrameTranslate(frame,&frame_obj.origin,rwCOMBINEREPLACE);
      RwFrameRotate(frame,&frame_obj.x_axis,FLIP_ANGLE_DEG,rwCOMBINEPRECONCAT);
      RwFrameRotate(frame,&frame_obj.y_axis,FLIP_ANGLE_DEG,rwCOMBINEPOSTCONCAT);
      RwFrameTranslate(frame,(RwV3d *)(viewport_camera + CAMERA_POS_OFFSET),rwCOMBINEPOSTCONCAT);
    }
    slot = slot + 1;
    slots_left = slots_left + -1;
  } while (slots_left != 0);
  return;
}


