#include "mfl_tier2.h"
/*00405e50*/ extern int __cdecl spline_tween_a(RwMatrix *p1, int p2, float p3);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*005dda1c*/ extern float k_f_3_3333333en4;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005dda24*/ extern float k_f_0_75;
/*005ddc28*/ extern float k_f_0;
/*00651240*/ extern undefined4 g_scene_selected_object;
/*00651244*/ extern float g_scene_select_timer;
/*00651248*/ extern RwCamera *g_scene_select_param;
/*00801b2c*/ extern float g_camera_fov_factor;
/* offset of the spline's end time (float) inside the selected scene object (inferred) */
#define SPLINE_END_TIME_OFF (0xc)

/* camera_path_update
 *
 * Moves the scene camera along the selected object's spline and re-aims it, one step per call.
 * Does nothing unless a scene object is selected (g_scene_selected_object) and a camera is set
 * (g_scene_select_param).
 *
 * time_step: signed step amount for this call; a negative value is read as unsigned (+2^32 as a
 *            float) before scaling (inferred).
 * On success also advances g_scene_select_timer by time_step * k_f_3_3333333en4 (about 1/3000).
 * Returns 1 if the camera was placed on the spline this call, 0 if the guard failed or the timer
 * is past the spline end time.
 *
 * (was: "moves the camera along a spline (DAT_00651240 / 51244 / 51248) and sets the view window
 *        0.75 / 0.75" - view window x is 0.75 / g_camera_fov_factor, not a fixed 0.75)
 */

undefined4 __cdecl camera_path_update(int time_step)

{
  RwFrame *camera_frame;
  float time_step_f;
  struct {
    RwV2d view_window;
    RwMatrix spline_matrix;
  } path_scratch;
  
  if ((g_scene_selected_object != 0) && (g_scene_select_param != (RwCamera *)0x0)) {
    /* set the view window every call: width scales with the camera FOV factor, height is fixed */
    path_scratch.view_window.x = k_f_0_75 / g_camera_fov_factor;
    path_scratch.view_window.y = 0.75;
    RwCameraSetViewWindow(g_scene_select_param,&path_scratch.view_window);
    camera_frame = RwCamera_object_object_parent(g_scene_select_param);
    /* timer still within the spline's end time: place the camera on the spline */
    if (g_scene_select_timer <= *(float *)(g_scene_selected_object + SPLINE_END_TIME_OFF)) {
      spline_tween_a(&path_scratch.spline_matrix,g_scene_selected_object,g_scene_select_timer);
      /* timer equal to k_f_0 (presumably 0.0f): RwFrameTransform is called here and again
         unconditionally below with the same arguments (inferred, check disassembly) */
      if (k_f_0 == g_scene_select_timer) {
        RwFrameTransform(camera_frame,&path_scratch.spline_matrix,rwCOMBINEREPLACE);
      }
      RwFrameTransform(camera_frame,&path_scratch.spline_matrix,rwCOMBINEREPLACE);
      /* advance the timer; a negative step is converted as an unsigned 32-bit value */
      time_step_f = (float)time_step;
      if (time_step < 0) {
        time_step_f = time_step_f + k_f_4_2949673e9;
      }
      g_scene_select_timer = time_step_f * k_f_3_3333333en4 + g_scene_select_timer;
      return 1;
    }
  }
  return 0;
}


