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

#if MFL_FIX_WIDESCREEN
float mfl_widescreen_camera_factor(void);
#endif

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
  *(float *)(camera + CAMERA_FOV_OFFSET) = mfl_widescreen_camera_factor();
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

/* WSVID wv1: the camera aspect factor for a camera that renders into a page element. The race-results replay (app state 1, menu 3) draws the
   course with the second camera into a 256 x 220 window that ui_camera_update_view_rect already keeps at its 4:3 shape (width squeezed by k);
   a window widened to the mode's ratio would squeeze the picture again (a 16:9 view window into a 1.15:1 rect = 0.64 of the original width).
   There the original 4:3 factor stays, so the picture is exactly the original one. Everywhere else = mfl_widescreen_fov_factor. */
/*0076e7f4*/ extern undefined4 g_app_state;
/*0069d1ec*/ extern int ui_get_menu_69d1ec(void);
float mfl_widescreen_camera_factor(void)
{
  if (g_app_state == APP_STATE_MAIN_LOOP_INIT && ui_get_menu_69d1ec() == 3)
    return g_camera_fov_factor;
  return mfl_widescreen_fov_factor();
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

/* Race phase 7 (standings, winner camera): camera_race_setup does not run camera_update, so the viewport camera keeps the 0.8 x 0.8 window
   that camera_set_default_view_window left after the HUD pass (app_state_menu_render) and the original draws the winner car with it.
   Same window at 4:3, wider by 0.75 / factor at a wide mode (Hor+), so the car keeps its 4:3 look. */
#define MFL_WS_DEFAULT_WINDOW 0.8f
void mfl_widescreen_standings_window(void *camera)
{
  RwV2d view_window;

  view_window.x = MFL_WS_DEFAULT_WINDOW * 0.75f / mfl_widescreen_fov_factor();
  view_window.y = MFL_WS_DEFAULT_WINDOW;
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

/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
#define MFL_WS_RWCAMERA_VIEW_WINDOW_X 0x68      /* RwCamera.viewWindow.x */

/* Game logic (physics-object triggers, course events, bomb trails) frustum-tests spheres against the race camera; the switch
   widens that camera's view window, the original tested the 4:3 one.  Same RW test, and a sphere that lies wholly outside one of
   the two 4:3 side planes (only in the extra side strips) is outside, as in the original.  Callers only tell outside from
   not outside.  Render culling keeps the wide test. */
int mfl_widescreen_frustum_test_sphere_43(void *camera, void *sphere)
{
  const float *s = (const float *)sphere;      /* centre x y z, radius */
  int result = RwCameraFrustumTestSphere((RwCamera *)camera, (RwSphere *)sphere);
  RwMatrix *ltm;
  float half_width, dx, dy, dz, side, depth, right_out, left_out, r2;

  if (result == rwSPHEREOUTSIDE)
    return result;
  ltm = RwFrameGetLTM((RwFrame *)RwCamera_object_object_parent((RwCamera *)camera));
  half_width = *(float *)((char *)camera + MFL_WS_RWCAMERA_VIEW_WINDOW_X) * mfl_widescreen_fov_factor() / 0.75f;
  dx = s[0] - ltm->pos.x;
  dy = s[1] - ltm->pos.y;
  dz = s[2] - ltm->pos.z;
  side = dx * ltm->right.x + dy * ltm->right.y + dz * ltm->right.z;
  depth = dx * ltm->at.x + dy * ltm->at.y + dz * ltm->at.z;
  right_out = side - half_width * depth;      /* > 0 past a side plane; its distance is this / sqrt(1 + half_width^2) */
  left_out = -side - half_width * depth;
  r2 = s[3] * s[3] * (1.0f + half_width * half_width);
  if ((right_out > 0.0f && right_out * right_out > r2) || (left_out > 0.0f && left_out * left_out > r2))
    return rwSPHEREOUTSIDE;
  return result;
}
#endif


