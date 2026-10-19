#include "mfl_tier2.h"
/*0044a410*/ extern int camera_update_view_window();
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*0062ac74*/ extern float g_car_62ac74;
/*00801b2c*/ extern float g_camera_fov_factor;

/* byte offsets into the camera object (not struct members; inferred from use) */
#define CAMERA_FOV_OFFSET (0x24)
#define CAMERA_CLUMP_OFFSET (0x84)
#define CLUMP_FRAME_OFFSET (4)
#define CAMERA_YAW_OFFSET (0x38)
#define CAMERA_PITCH_OFFSET (0x34)
#define CAMERA_ROLL_OFFSET (0x3c)
#define CAMERA_TRANSLATE_OFFSET (0x40)

/* camera_apply_rotation */

/* "sets fov, view window and rotates the camera frame by pitch / yaw" */

/*
 * Applies the camera's view settings: copies the fov factor (float bits) into the camera's
 * fov slot, refreshes the view window, then rebuilds the clump's frame from identity:
 * rotate about the global axis g_car_62ac74 (yaw, inferred), then about the frame's right
 * and at vectors (pitch and roll, inferred), then translate by the camera offset vector.
 * camera is the camera object; its offsets are inferred from use.
 */
void camera_apply_rotation(int camera)

{
  RwMatrix *matrix;
  RwFrame *frame;

  /* fov factor: float bits copied into the camera's fov slot */
  *(undefined4 *)(camera + CAMERA_FOV_OFFSET) = *(int *)&g_camera_fov_factor;
  camera_update_view_window(camera);
  /* the clump's frame, reset to identity before the rotations are applied */
  frame = *(RwFrame **)(*(char **)(camera + CAMERA_CLUMP_OFFSET) + CLUMP_FRAME_OFFSET);
  RwFrameSetIdentity(frame);
  matrix = &RwFrame_modelling(frame);
  /* rotations are post-concatenated in this order: yaw, pitch, roll */
  RwMatrixRotate(matrix,(RwV3d *)&g_car_62ac74,*(RwReal *)(camera + CAMERA_YAW_OFFSET),rwCOMBINEPOSTCONCAT);
  RwMatrixRotate(matrix,&matrix->right,*(RwReal *)(camera + CAMERA_PITCH_OFFSET),rwCOMBINEPOSTCONCAT);
  RwMatrixRotate(matrix,&(RwFrame_modelling(frame)).at,*(RwReal *)(camera + CAMERA_ROLL_OFFSET),rwCOMBINEPOSTCONCAT);
  RwFrameTranslate(frame,(RwV3d *)(camera + CAMERA_TRANSLATE_OFFSET),rwCOMBINEPOSTCONCAT);
  return;
}


