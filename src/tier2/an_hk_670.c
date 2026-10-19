#include "mfl_tier2.h"
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*0060d738*/ extern undefined g_course_60d738;
/*0060d768*/ extern undefined4 g_course_60d768;
/*006a0784*/ extern float g_course_arctic_6a0784;
/*006a0788*/ extern float g_course_arctic_6a0788;
/*006a078c*/ extern float g_course_arctic_6a078c;
/*008c5910*/ extern undefined4 g_camera_8c5910;
extern double cos(double);
extern double sin(double);
/* tilt amplitude passed as the RwFrameRotate angle (inferred: degrees) */
#define TILT_AMPLITUDE (5.0)
/* amplitude of the vertical bob added to the translation y (inferred) */
#define BOB_AMPLITUDE (0.25)
/* constant subtracted from the bob before it is stored as translation y (inferred) */
#define BOB_Y_OFFSET (4.0)
/* byte stride between entries of the camera pointer and angle tables */
#define SLOT_STRIDE (4)
/* offset of the frame pointer inside a clump (inferred: RpClump frame field) */
#define CLUMP_FRAME_OFFSET (4)
/* course_arctic_frames_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "rotates the course frames of the arctic course from the angle tables" */

/*
 * Per-frame update for the Arctic course: for each of the 4 camera slots
 * (inferred from the loop bound), resets the clump's frame to identity, turns it
 * about the Y axis by the angle in g_course_60d768, tilts it by TILT_AMPLITUDE
 * scaled cos/sin of the phases in g_course_arctic_6a078c / 6a0788, and translates
 * it by a bob from g_course_arctic_6a0784 (y only, written into g_course_60d738).
 * Takes no parameters and returns nothing.
 */
void course_arctic_frames_update(void)

{
  RwFrame *frame;
  char *clump;
  int byte_offset;
  RwV3d *translation;
  float10 trig_value;
  RwV3d y_axis;

  y_axis.x = 0.0;
  y_axis.y = 1.0;
  y_axis.z = 0.0;
  translation = (RwV3d *)&g_course_60d738;
  byte_offset = 0;
  /* one pass per camera slot: update that clump's frame and its translation entry */
  do {
    frame = *(RwFrame **)(*(char **)((char *)&g_camera_8c5910 + byte_offset) + CLUMP_FRAME_OFFSET);
    RwFrameSetIdentity(frame);
    RwFrameRotate(frame,&y_axis,*(RwReal *)((char *)&g_course_60d768 + byte_offset),rwCOMBINEREPLACE);
    trig_value = (float10)cos((float10)g_course_arctic_6a078c);
    RwFrameRotate(frame,&(RwFrame_modelling(frame)).at,(float)(trig_value * (float10)(TILT_AMPLITUDE)),
                  rwCOMBINEPOSTCONCAT);
    trig_value = (float10)sin((float10)g_course_arctic_6a0788);
    RwFrameRotate(frame,&(RwFrame_modelling(frame)).right,(float)(trig_value * (float10)(TILT_AMPLITUDE)),
                  rwCOMBINEPOSTCONCAT);
    trig_value = (float10)sin((float10)g_course_arctic_6a0784);
    clump = *(char **)((char *)&g_camera_8c5910 + byte_offset);
    translation->y = (float)(trig_value * (float10)(BOB_AMPLITUDE) - (float10)(BOB_Y_OFFSET));
    RwFrameTranslate(*(RwFrame **)(clump + CLUMP_FRAME_OFFSET),translation,rwCOMBINEPOSTCONCAT);
    translation = translation + 1;
    byte_offset = byte_offset + SLOT_STRIDE;
  } while ((int)translation < ((int)&g_course_60d768));
  return;
}


