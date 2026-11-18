#include "mfl_tier2.h"
#include "mfl_fix.h"
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
void camera_apply_rotation(char *camera)

{
  RwMatrix *matrix;
  RwFrame *frame;

  /* fov factor: float bits copied into the camera's fov slot */
#if MFL_FIX_WIDESCREEN
  *(float *)(camera + CAMERA_FOV_OFFSET) = mfl_widescreen_fov_factor();
#else
  *(undefined4 *)(camera + CAMERA_FOV_OFFSET) = *(int *)&g_camera_fov_factor;
#endif
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

#if MFL_FIX_WIDESCREEN
/*004a8830*/ extern int app_get_app_62c5c8(void);
/*004a8840*/ extern int app_get_app_62c5cc(void);
/* MFL_FIX_WIDESCREEN: the camera aspect factor for the current video mode, height / width of the frame buffer
   (app_get_app_62c5c8 / 62c5cc, set from the mode by the video setup). 640x480 gives 0.75, the original value; the
   view window of camera_update_view_window then keeps its height and widens to the mode's ratio (Hor+).
   Before the mode is known the original global is returned. */
float mfl_widescreen_fov_factor(void)
{
  int width = app_get_app_62c5c8();
  int height = app_get_app_62c5cc();

  if (width <= 0 || height <= 0)
    return g_camera_fov_factor;
  return (float)height / (float)width;
}

/* Step 2, the in-race HUD (EDGES layout). The HUD camera (camera_save: orthographic, view window 0.6 x 0.45 = 4:3) gets the mode's ratio,
   0.45 / factor wide, so panels and the Rt2d text keep their proportions; at 4:3 every value below is the original one. */
#define MFL_WS_HUD_HALF_HEIGHT 0.45f
#define MFL_WS_HUD_HALF_WIDTH_43 0.6f
#define MFL_WS_HUD_CENTRE_ZONE 0.2f      /* panels with |x| <= this stay centred; the others move with their side edge */
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);

/* the HUD camera window for the current mode; called right after camera_save (and after Rt2dDeviceSetCamera, so the Rt2d layer keeps its
   4:3 coordinates and is drawn unstretched in the middle); camera_restore puts the saved window back */
void mfl_widescreen_hud_camera(void *camera)
{
  RwV2d view_window;

  view_window.x = MFL_WS_HUD_HALF_HEIGHT / mfl_widescreen_fov_factor();
  view_window.y = MFL_WS_HUD_HALF_HEIGHT;
  RwCameraSetViewWindow((RwCamera *)camera, &view_window);
}

/* screen-tracking HUD items (player markers): their 4:3 x range +-0.6 becomes the full width */
float mfl_widescreen_hud_scale(void)
{
  return MFL_WS_HUD_HALF_HEIGHT / mfl_widescreen_fov_factor() / MFL_WS_HUD_HALF_WIDTH_43;
}

/* RwMatrixTranslate for a HUD panel origin: x moves out by the extra half width when the panel sits left or right of the centre zone */
void *mfl_widescreen_hud_translate(void *matrix, const void *origin, int combine)
{
  RwV3d v = *(const RwV3d *)origin;
  float shift = MFL_WS_HUD_HALF_HEIGHT / mfl_widescreen_fov_factor() - MFL_WS_HUD_HALF_WIDTH_43;

  if (v.x > MFL_WS_HUD_CENTRE_ZONE)
    v.x = v.x + shift;
  else if (v.x < -MFL_WS_HUD_CENTRE_ZONE)
    v.x = v.x - shift;
  return RwMatrixTranslate((RwMatrix *)matrix, &v, (enum RwOpCombineType)combine);
}
#endif


