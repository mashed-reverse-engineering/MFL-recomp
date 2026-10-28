#include "mfl_tier2.h"
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004f6bf0*/ extern int rpclump_clone(undefined4 p0);
/*0060d718*/ extern undefined g_course_60d718;
/*0060d730*/ extern undefined4 g_course_60d730;
/*008c5920*/ extern undefined4 g_camera_8c5920;
/*008c5924*/ extern undefined4 g_course_8c5924;
#define WORLD_RECORD_CLUMP_C11C_OFFSET (0xc11c) /* inferred: a clump (rendered with RpClumpRender in an_hk_042), not a camera */
#define CAMERA_FRAME_OFFSET (4)
/* camera_create_a */
/* "creates the course camera 0xC11C of the world record" */
/*
 * Creates the course camera for a world record.
 * world_record: the record; its clump pointer (inferred) is at WORLD_RECORD_CLUMP_C11C_OFFSET.
 * Stores that camera in g_camera_8c5920, resets its frame to identity, and creates
 * the second camera g_course_8c5924 from it (rpclump_clone, inferred).
 * Returns nothing.
 */
void camera_create_a(char *world_record)
{
  int slot_offset;
  RwV3d *translation;
  RwV3d up_axis;

  /* the course camera; its frame is at CAMERA_FRAME_OFFSET */
  /* up vector (0, 1, 0) used as the rotation axis */
  up_axis.x = 0.0;
  up_axis.y = 1.0;
  up_axis.z = 0.0;
  g_camera_8c5920 = *(int *)(world_record + WORLD_RECORD_CLUMP_C11C_OFFSET);
  RwFrameSetIdentity(*(RwFrame **)(g_camera_8c5920 + CAMERA_FRAME_OFFSET));
  g_course_8c5924 = rpclump_clone(g_camera_8c5920);
  translation = (RwV3d *)&g_course_60d718;
  slot_offset = 0;
  /* for each of the two cameras (slot_offset 0 and 4, inferred): reset its frame,
     rotate it about up_axis by its angle in g_course_60d730 (inferred), then
     translate it by the next RwV3d in g_course_60d718 (inferred) */
  do {
    RwFrameSetIdentity(*(RwFrame **)(*(char **)((char *)&g_camera_8c5920 + slot_offset) + CAMERA_FRAME_OFFSET));
    RwFrameRotate(*(RwFrame **)(*(char **)((char *)&g_camera_8c5920 + slot_offset) + CAMERA_FRAME_OFFSET),&up_axis,
                  *(RwReal *)((char *)&g_course_60d730 + slot_offset),rwCOMBINEREPLACE);
    RwFrameTranslate(*(RwFrame **)(*(char **)((char *)&g_camera_8c5920 + slot_offset) + CAMERA_FRAME_OFFSET),translation,
                     rwCOMBINEPOSTCONCAT);
    translation = translation + 1;
    slot_offset = slot_offset + 4;
  } while ((LONG_PTR)translation < (LONG_PTR)&g_course_60d730);
  return;
}
