#include "mfl_tier2.h"
/*0042ab20*/ extern undefined __cdecl course_camera_add_to_collision_world(struct RwCamera *p0);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1da0*/ extern struct RwCamera * RwCameraSetProjection(struct RwCamera *p0, enum RwCameraProjection p1);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*00652e08*/ extern undefined4 g_camera_652e08;
/*00652e88*/ extern undefined4 g_camera_652e88;
/*00652e8c*/ extern undefined4 g_camera_652e8c;
/*00652e90*/ extern undefined4 g_camera_save_652e90;
#define VIEW_WINDOW_X (0.6)
#define VIEW_WINDOW_Y (0.45)
#define MATRIX_FLAGS_IDENTITY_ORTHONORMAL (0x20003)
/* camera_save */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/*
 * Saves the camera's state into globals, then resets the camera to a fixed setup:
 * identity frame, 0.6 x 0.45 view window, parallel projection, and adds it to the
 * course collision world.
 * camera: the RwCamera to save and reset.
 * Saved: frame->modelling -> g_camera_652e08, view window x -> g_camera_652e8c,
 * view window y -> g_camera_save_652e90, projectionType -> g_camera_652e88.
 * (was: "(camera): copies the camera frame matrix into 0x652E08 and sets the 0.6 / 0.45 view window")
 */
void camera_save(RwCamera *camera)
{
  RwFrame *frame;
  struct {
    RwV2d view_window;
    RwMatrix identity_matrix;
  } setup;
  
  frame = RwCamera_object_object_parent(camera);
  setup.view_window.x = VIEW_WINDOW_X;
  setup.view_window.y = VIEW_WINDOW_Y;
  /* save the current frame matrix, view window and projection into the globals */
  *(RwMatrix *)&g_camera_652e08 = RwFrame_modelling(frame);
  g_camera_652e8c = *(undefined4 *)&(RwCamera_viewWindow(camera)).x;
  g_camera_save_652e90 = *(undefined4 *)&(RwCamera_viewWindow(camera)).y;
  g_camera_652e88 = RwCamera_projectionType(camera);
  /* new setup: identity matrix (flags: identity + orthonormal type), then apply it to the frame */
  setup.identity_matrix.at.z = 1.0;
  setup.identity_matrix.up.y = 1.0;
  setup.identity_matrix.right.x = 1.0;
  setup.identity_matrix.up.x = 0.0;
  setup.identity_matrix.right.z = 0.0;
  setup.identity_matrix.right.y = 0.0;
  setup.identity_matrix.at.y = 0.0;
  setup.identity_matrix.at.x = 0.0;
  setup.identity_matrix.up.z = 0.0;
  setup.identity_matrix.pos.z = 0.0;
  setup.identity_matrix.pos.y = 0.0;
  setup.identity_matrix.pos.x = 0.0;
  setup.identity_matrix.flags = setup.identity_matrix.flags | MATRIX_FLAGS_IDENTITY_ORTHONORMAL;
  RwFrameTransform(frame,&setup.identity_matrix,rwCOMBINEREPLACE);
  RwCameraSetViewWindow(camera,&setup.view_window);
  RwCameraSetProjection(camera,rwPARALLEL);
  course_camera_add_to_collision_world(camera);
  return;
}
