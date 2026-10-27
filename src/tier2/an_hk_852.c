#include "mfl_tier2.h"
/*0044a470*/ extern int camera_apply_rotation();
/*004d1da0*/ extern struct RwCamera * RwCameraSetProjection(struct RwCamera *p0, enum RwCameraProjection p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5410*/ extern RwReal _rwSqrt(RwReal p0);
/*005ddacc*/ extern float k_f_360;
/*005ddad0*/ extern float k_f_180;
/*005ddad4*/ extern float k_f_90;
/*005ddc28*/ extern float k_f_0;
/*005de4ac*/ extern float k_f_270;
/*007f0240*/ extern float g_ai_editor_view_origin_x;
/*007f0244*/ extern float g_ai_editor_view_origin_z;
extern double atan2(double, double);
#define CAM_PITCH_DEG_OFF (0x34) /* (inferred) */
#define CAM_YAW_DEG_OFF (0x38) /* (inferred) */
#define CAM_ROLL_DEG_OFF (0x3c) /* (inferred) */
#define CAM_EYE_X_OFF (0x40) /* (inferred) */
#define CAM_EYE_Y_OFF (0x44) /* (inferred) */
#define CAM_EYE_Z_OFF (0x48) /* (inferred) */
#define CAM_RWCAMERA_PTR_OFF (0x84) /* (inferred) */
#define EDITOR_CAM_EYE_HEIGHT (0x41700000) /* float 15.0f (inferred) */
#define RAD_TO_DEG (57.295799255371094) /* 180 / pi */
/* camera_editor_setup_parallel */

/* "parallel projection over the AI editor view origin (g_ai_editor_view_origin_x / _z)" (was: "... AI grid bounds (g_ai_7f0240 / 7f0244)") */

/*
 * Switches the editor camera to parallel (orthographic) projection and places
 * its eye at the AI editor view origin (x, z), 15 units above the ground (inferred).
 * Then computes the camera angles in degrees and applies them with camera_apply_rotation():
 *   +0x34 pitch = 360 - elevation of the (dx, dy, dz) vector (inferred)
 *   +0x38 yaw   = angle of the (dx, dz) ground vector (inferred)
 *   +0x3c roll  = 0
 * cam_state is the editor camera object; its +0x84 holds the RwCamera *.
 * Note (inferred, check disassembly): the eye is written before dx and dz are
 * computed, so dx and dz come out as 0 and the angles are constant.
 */
void __cdecl camera_editor_setup_parallel(char *cam_state)

{
  float dy;
  float dz;
  double full_turn;
  double angle;
  RwReal horiz_len;
  RwV3d delta;
  float dx;

  /* dx and dz start as the view origin x / z, then become the deltas from the eye */
  dz = g_ai_editor_view_origin_z;
  dx = g_ai_editor_view_origin_x;
  *(float *)(cam_state + CAM_EYE_X_OFF) = g_ai_editor_view_origin_x;
  *(undefined4 *)(cam_state + CAM_EYE_Y_OFF) = EDITOR_CAM_EYE_HEIGHT;
  *(float *)(cam_state + CAM_EYE_Z_OFF) = dz;
  RwCameraSetProjection(*(RwCamera **)(cam_state + CAM_RWCAMERA_PTR_OFF),rwPARALLEL);
  dx = dx - *(float *)(cam_state + CAM_EYE_X_OFF);
  /* +0x58: unknown field, written 4.0f and not read in this function (inferred) */
  *(undefined4 *)(cam_state + 0x58) = 0x40800000;
  /* delta = view origin - eye; the origin is at y = 0 */
  dy = -*(float *)(cam_state + CAM_EYE_Y_OFF);
  dz = dz - *(float *)(cam_state + CAM_EYE_Z_OFF);
  delta.x = dx;
  delta.y = dy;
  delta.z = dz;
  RwV3dLength(&delta);
  /* yaw: angle of the (dx, dz) ground vector, in degrees (inferred) */
  if (dz == k_f_0) {
    if (dx < k_f_0) {
      angle = (double)k_f_270;
    }
    else {
      angle = (double)k_f_90;
    }
  }
  else {
    angle = (double)atan2((double)dx / (double)dz,(double)1);
    angle = (double)k_f_180 - -(angle * (double)(RAD_TO_DEG));
    if (k_f_0 < dz) {
      angle = angle + (double)k_f_180;
    }
  }
  *(float *)(cam_state + CAM_YAW_DEG_OFF) = (float)angle;
  /* dx is reused here: squared horizontal length dx*dx + dz*dz */
  dx = dx * dx + dz * dz;
  /* pitch: elevation of delta above the ground, in degrees (inferred) */
  if (dx != k_f_0) {
    horiz_len = _rwSqrt(dx);
    if (horiz_len != k_f_0) {
      angle = (double)atan2((double)dy / (double)horiz_len,(double)1);
      angle = (double)k_f_180 - -(angle * (double)(RAD_TO_DEG));
      if (k_f_0 < horiz_len) {
        angle = angle + (double)k_f_180;
      }
      goto LAB_0044a815;
    }
  }
  if (dy < k_f_0) {
    angle = (double)k_f_270;
  }
  else {
    angle = (double)k_f_90;
  }
  /* shared exit: pitch = 360 - angle */
LAB_0044a815:
  full_turn = (double)k_f_360;
  *(undefined4 *)(cam_state + CAM_ROLL_DEG_OFF) = 0;
  *(float *)(cam_state + CAM_PITCH_DEG_OFF) = (float)(full_turn - angle);
  camera_apply_rotation(cam_state);
  return;
}


