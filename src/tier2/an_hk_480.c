#include "mfl_tier2.h"
/*0042b200*/ extern int course_get_68b8b4_ptr();
/*004757a0*/ extern void * get_viewport_camera_matrix();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd328*/ extern float k_f_0_6;
/*005dd330*/ extern float float_1_0;
/*0064d880*/ extern undefined4 *g_cup_64d880;
/*0064d884*/ extern float g_cup_scene_64d884;
#define CUP_FRAME_OFFSET (4)
#define CUP_SCALE_SLOT (0xd)
#define LIGHT_SCALE_DEFAULT (2.0)
#define LIGHT_OFFSET_X (0.4)
#define LIGHT_OFFSET_Y (0.05)
#define LIGHT_OFFSET_Z (1.0)
#define CUP_TILT_ANGLE (-30.0)
#define MATRIX_FLAGS_IDENTITY_ORTHONORMAL (0x20003)
/* cup_scene_setup_lights */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "inferred: lights / scale constants of the cup scene" */

/*
 * Positions the cup scene's frame every time it runs. Takes no parameters, returns nothing.
 * Does nothing unless the cup object exists (*g_cup_64d880 != 0).
 * Otherwise it rebuilds the frame matrix as: identity, translate to the cup position,
 * tilt by CUP_TILT_ANGLE about the right axis, spin about the up axis by g_cup_scene_64d884
 * (a counter that grows by float_1_0 each call, inferred), scale by the cup scale,
 * translate by the light offset, then applies the camera matrix to the frame.
 */
void cup_scene_setup_lights(void)

{
  RwMatrix *frame_matrix;
  RwUInt32 *frame_flags;
  RwFrame *cup_frame;
  RwMatrix *camera_matrix;
  RwV3d *cup_position;
  struct {
    RwV3d scale;
    RwV3d offset;
  } light_xform;

  light_xform.offset.x = LIGHT_OFFSET_X;
  light_xform.offset.y = LIGHT_OFFSET_Y;
  light_xform.offset.z = LIGHT_OFFSET_Z;
  light_xform.scale.x = LIGHT_SCALE_DEFAULT;
  light_xform.scale.y = LIGHT_SCALE_DEFAULT;
  light_xform.scale.z = LIGHT_SCALE_DEFAULT;
  /* Result of course_get_68b8b4_ptr() is not used here; the call is kept for its side effects. */
  course_get_68b8b4_ptr();
  /* Cup object present: rebuild the cup frame transform. */
  if (*g_cup_64d880 != 0) {
    cup_frame = *(RwFrame **)(*g_cup_64d880 + CUP_FRAME_OFFSET);
    light_xform.scale.x = *(float *)&g_cup_64d880[CUP_SCALE_SLOT] * k_f_0_6;
    frame_matrix = &RwFrame_modelling(cup_frame);
    light_xform.scale.y = *(float *)&g_cup_64d880[CUP_SCALE_SLOT] * k_f_0_6;
    light_xform.scale.z = *(float *)&g_cup_64d880[CUP_SCALE_SLOT] * k_f_0_6;
    camera_matrix = (RwMatrix *)get_viewport_camera_matrix(0);
    g_cup_scene_64d884 = g_cup_scene_64d884 + float_1_0;
    (RwFrame_modelling(cup_frame)).at.z = 1.0;
    (RwFrame_modelling(cup_frame)).up.y = 1.0;
    (frame_matrix->right).x = 1.0;
    frame_flags = &(RwFrame_modelling(cup_frame)).flags;
    *frame_flags = *frame_flags | MATRIX_FLAGS_IDENTITY_ORTHONORMAL;
    cup_position = (RwV3d *)(g_cup_64d880 + 1);
    /* Reset the frame matrix to identity: right=(1,0,0), up=(0,1,0), at=(0,0,1), pos=0. */
    (RwFrame_modelling(cup_frame)).up.x = 0.0;
    (RwFrame_modelling(cup_frame)).right.z = 0.0;
    (RwFrame_modelling(cup_frame)).right.y = 0.0;
    (RwFrame_modelling(cup_frame)).at.y = 0.0;
    (RwFrame_modelling(cup_frame)).at.x = 0.0;
    (RwFrame_modelling(cup_frame)).up.z = 0.0;
    (RwFrame_modelling(cup_frame)).pos.z = 0.0;
    (RwFrame_modelling(cup_frame)).pos.y = 0.0;
    (RwFrame_modelling(cup_frame)).pos.x = 0.0;
    /* Build the cup transform: position, tilt, spin, scale, light offset (see the function comment). */
    RwMatrixTranslate(frame_matrix,cup_position,rwCOMBINEREPLACE);
    RwMatrixRotate(frame_matrix,&frame_matrix->right,CUP_TILT_ANGLE,rwCOMBINEPOSTCONCAT);
    RwMatrixRotate(frame_matrix,&(RwFrame_modelling(cup_frame)).up,g_cup_scene_64d884,rwCOMBINEPOSTCONCAT);
    RwMatrixScale(frame_matrix,&light_xform.scale,rwCOMBINEPOSTCONCAT);
    RwMatrixTranslate(frame_matrix,&light_xform.offset,rwCOMBINEPOSTCONCAT);
    RwFrameTransform(cup_frame,camera_matrix,rwCOMBINEPOSTCONCAT);
  }
  return;
}


