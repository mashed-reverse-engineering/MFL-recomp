#include "mfl_tier2.h"
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*005dd338*/ extern float k_f_0_01;
/*005de2fc*/ extern float k_f_0_04;
/*005de490*/ extern float k_f_0_02;
/*0060d718*/ extern undefined g_course_60d718;
/*0060d730*/ extern undefined4 g_course_60d730;
/*006a0784*/ extern float g_course_arctic_6a0784;
/*006a0788*/ extern float g_course_arctic_6a0788;
/*006a078c*/ extern float g_course_arctic_6a078c;
/*008c5920*/ extern undefined4 g_camera_8c5920;
extern double cos(double);
extern double sin(double);
/* sway tuning (inferred from the sin/cos uses below) */
#define SWAY_ROT_AMPLITUDE (5.0) /* rotation about modelling.at / right, degrees (inferred) */
#define SWAY_HEIGHT_AMPLITUDE (0.5) /* sin() amplitude of the y translation */
#define SWAY_HEIGHT_OFFSET (4.5) /* constant y translation, subtracted */
/* course_arctic_frames_sway_update */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "sways the frames of the second object set with sin/cos of three slowly advancing phases" */
/* Per-frame sway for the two objects in g_camera_8c5920: each frame is reset to identity, turned by its
   base angle about the up axis (y), then tilted about its modelling at/right vectors by cos/sin of two phases,
   and set to a sin-driven height (third phase) in the shared translation vector. The three phases are advanced once per call.
   Parameters: none. Returns nothing. */
void course_arctic_frames_sway_update(void)
{
  RwFrame *frame;
  char *sway_object;
  int object_offset;
  RwV3d *sway_vec;
  float10 trig_value;
  RwV3d up_axis;

  up_axis.x = 0.0;
  up_axis.y = 1.0;
  up_axis.z = 0.0;
  sway_vec = (RwV3d *)&g_course_60d718;
  object_offset = 0;
  /* loop over the two objects: object_offset steps 4 bytes through the pointer table,
     sway_vec steps one RwV3d (12 bytes) through the translation array in g_course_60d718 */
  do {
    frame = *(RwFrame **)(*(char **)((char *)&g_camera_8c5920 + object_offset) + 4);
    RwFrameSetIdentity(frame);
    RwFrameRotate(frame,&up_axis,*(RwReal *)((char *)&g_course_60d730 + object_offset),rwCOMBINEREPLACE);
    trig_value = (float10)cos((float10)g_course_arctic_6a078c);
    RwFrameRotate(frame,&(RwFrame_modelling(frame)).at,(float)(trig_value * (float10)(SWAY_ROT_AMPLITUDE)),
                  rwCOMBINEPOSTCONCAT);
    trig_value = (float10)sin((float10)g_course_arctic_6a0788);
    RwFrameRotate(frame,&(RwFrame_modelling(frame)).right,(float)(trig_value * (float10)(SWAY_ROT_AMPLITUDE)),
                  rwCOMBINEPOSTCONCAT);
    trig_value = (float10)sin((float10)g_course_arctic_6a0784);
    sway_object = *(char **)((char *)&g_camera_8c5920 + object_offset);
    sway_vec->y = (float)(trig_value * (float10)(SWAY_HEIGHT_AMPLITUDE) - (float10)(SWAY_HEIGHT_OFFSET));
    RwFrameTranslate(*(RwFrame **)(sway_object + 4),sway_vec,rwCOMBINEPOSTCONCAT);
    sway_vec = sway_vec + 1;
    object_offset = object_offset + 4;
  } while ((int)sway_vec < ((int)&g_course_60d730));
  /* advance the three sway phases */
  g_course_arctic_6a0784 = g_course_arctic_6a0784 + k_f_0_02;
  g_course_arctic_6a078c = g_course_arctic_6a078c + k_f_0_04;
  g_course_arctic_6a0788 = g_course_arctic_6a0788 + k_f_0_01;
  return;
}
