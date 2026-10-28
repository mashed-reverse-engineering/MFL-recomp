#include "mfl_tier2.h"
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004f6bf0*/ extern int rpclump_clone(undefined4 p0);
/*0060d738*/ extern undefined g_course_60d738;
/*0060d768*/ extern undefined4 g_course_60d768;
/*008c5910*/ extern undefined4 g_camera_8c5910;
/*008c5914*/ extern undefined4 g_course_8c5914;
/*008c5920*/ extern char g_camera_8c5920[];
#define WORLD_CLUMP_C128_OFFSET (0xc128) /* inferred: a clump, not a camera */
#define CAMERA_FRAME_OFFSET (4)
/* camera_create_b */
/* "creates the camera at +0xC128 of the world record"
 * Stores the clump pointer (inferred) from world + WORLD_CLUMP_C128_OFFSET in g_camera_8c5910 and resets its frame to identity.
 * Fills the three slots g_course_8c5914..g_course_8c591c with rpclump_clone(camera) (inferred: per-camera setup).
 * Then, for each of the four camera slots (g_camera_8c5910 .. +0xc), resets the frame, rotates it about the
 * up axis (0, 1, 0) by an angle from g_course_60d768 (inferred), and post-translates it by the matching
 * RwV3d from g_course_60d738 (inferred).
 * world: pointer to the world record. Returns nothing. */
void camera_create_b(char *world)
{
  undefined4 slot_value;
  undefined4 *slot;
  int byte_offset;
  RwV3d *translation;
  RwV3d up_axis;

  up_axis.x = 0.0;
  up_axis.y = 1.0;
  up_axis.z = 0.0;
  g_camera_8c5910 = *(int *)(world + WORLD_CLUMP_C128_OFFSET);
  RwFrameSetIdentity(*(RwFrame **)(g_camera_8c5910 + CAMERA_FRAME_OFFSET));
  /* fill the slots after the camera pointer, one call per slot (inferred) */
  slot = &g_course_8c5914;
  do {
    slot_value = rpclump_clone(g_camera_8c5910);
    *slot = slot_value;
    slot = slot + 1;
  } while ((int)slot < (int)&g_camera_8c5920);
  /* per camera: identity, rotate about up_axis, then translate (inferred) */
  translation = (RwV3d *)&g_course_60d738;
  byte_offset = 0;
  do {
    RwFrameSetIdentity(*(RwFrame **)(*(char **)((char *)&g_camera_8c5910 + byte_offset) + CAMERA_FRAME_OFFSET));
    RwFrameRotate(*(RwFrame **)(*(char **)((char *)&g_camera_8c5910 + byte_offset) + CAMERA_FRAME_OFFSET),&up_axis,
                  *(RwReal *)((char *)&g_course_60d768 + byte_offset),rwCOMBINEREPLACE);
    RwFrameTranslate(*(RwFrame **)(*(char **)((char *)&g_camera_8c5910 + byte_offset) + CAMERA_FRAME_OFFSET),translation,
                     rwCOMBINEPOSTCONCAT);
    translation = translation + 1;
    byte_offset = byte_offset + 4;
  } while ((int)translation < (int)&g_course_60d768);
  return;
}
