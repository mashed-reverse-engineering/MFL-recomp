// cluster-entries: camera_debug_adjust_scale
/* Installed cluster camera_debug_adjust_scale (tools/cluster_install.py from output/cluster_pilot_zy/camera_debug_adjust_scale.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*0060af50*/ extern float g_camera_debug_60af50;
/*008c7e59*/ extern signed char g_input_8c7e59;
/*008c7e5a*/ extern signed char g_input_8c7e5a;
/*008c7e5b*/ extern signed char g_input_8c7e5b;
/*008c7e58*/ extern signed char g_controller_8c7e58;
/*008c7e50*/ extern float g_camera_debug_8c7e50;
/*008c7e34*/ extern float g_camera_debug_8c7e34;
/*0062ac74*/ extern struct RwV3d g_car_62ac74;
/*005e64a4*/ extern float k_f_0;
/*005dd338*/ extern float k_f_0_01;
/*005df2d4*/ extern float k_f_1_333;
/*005dd364*/ extern float k_f_n1;
/*005df2d0*/ extern float k_f_n48;
/*005de9a4*/ extern float k_f_48;
/*005df2cc*/ extern float k_f_n0_015625;
/*005df2c8*/ extern float k_f_0_002;
/*005de19c*/ extern float k_f_1_0en4;
/*005ddacc*/ extern float k_f_360;
/*004d4870*/ extern RwMatrix *RwMatrixTranslate(RwMatrix *matrix, struct RwV3d *translation, enum RwOpCombineType combine);
/*004d43f0*/ extern RwMatrix *RwMatrixRotate(RwMatrix *matrix, struct RwV3d *axis, RwReal angle, enum RwOpCombineType combine);
/*004d1e10*/ extern struct RwCamera *RwCameraSetViewWindow(struct RwCamera *camera, struct RwV2d *view_window);

/* Debug free camera (player 2 input): fire / drop change the view window scale by +-0.01 (minimum 0.01); two stick axes rotate the camera frame (yaw about the world axis g_car_62ac74, pitch about its right vector, wrapped 0..360, dead zone +-48);
   two more axes move it along its at / right vectors (slower while insult is held).  `camera` is the RwCamera whose parent frame is moved. */
void camera_debug_adjust_scale(struct RwCamera *camera)
{
  struct RwV2d window;
  struct RwV3d position;
  struct RwV3d back;
  struct RwV3d step;
  char *frame;
  RwMatrix *matrix;
  float angle;
  float speed;
  double along;  /* kept unrounded on the x87 stack by the original */

  if (g_player_inputs[1].fire != 0) {
    g_camera_debug_60af50 = g_camera_debug_60af50 + k_f_0_01;
  }
  if (g_player_inputs[1].drop != 0) {
    g_camera_debug_60af50 = g_camera_debug_60af50 - k_f_0_01;
  }
  if (g_camera_debug_60af50 < k_f_0_01) {
    g_camera_debug_60af50 = 0.01f;
  }
  window.x = g_camera_debug_60af50 * k_f_1_333;
  window.y = g_camera_debug_60af50;
  RwCameraSetViewWindow(camera, &window);
  frame = *(char **)((char *)camera + 4);
  matrix = (RwMatrix *)(frame + 0x10);
  position.x = *(float *)(frame + 0x40);
  back.x = position.x * k_f_n1;
  position.y = *(float *)(frame + 0x44);
  position.z = *(float *)(frame + 0x48);
  back.y = position.y * k_f_n1;
  back.z = position.z * k_f_n1;
  RwMatrixTranslate(matrix, &back, rwCOMBINEPOSTCONCAT);
  angle = (float)(int)g_input_8c7e59;
  if (k_f_n48 < angle && angle < k_f_48) {
    angle = k_f_0;
  }
  angle = angle * k_f_n0_015625;
  RwMatrixRotate(matrix, &g_car_62ac74, angle, rwCOMBINEPOSTCONCAT);
  g_camera_debug_8c7e50 = g_camera_debug_8c7e50 + angle;
  if (k_f_360 < g_camera_debug_8c7e50) {
    g_camera_debug_8c7e50 = g_camera_debug_8c7e50 - k_f_360;
  }
  if (g_camera_debug_8c7e50 < k_f_0) {
    g_camera_debug_8c7e50 = g_camera_debug_8c7e50 + k_f_360;
  }
  angle = (float)(int)g_controller_8c7e58;
  if (k_f_n48 < angle && angle < k_f_48) {
    angle = k_f_0;
  }
  angle = angle * k_f_n0_015625;
  RwMatrixRotate(matrix, (struct RwV3d *)matrix, angle, rwCOMBINEPOSTCONCAT);
  g_camera_debug_8c7e34 = g_camera_debug_8c7e34 + angle;
  if (k_f_360 < g_camera_debug_8c7e34) {
    g_camera_debug_8c7e34 = g_camera_debug_8c7e34 - k_f_360;
  }
  if (g_camera_debug_8c7e34 < k_f_0) {
    g_camera_debug_8c7e34 = g_camera_debug_8c7e34 + k_f_360;
  }
  RwMatrixTranslate(matrix, &position, rwCOMBINEPOSTCONCAT);
  along = -(float)(int)g_input_8c7e5a;
  if (k_f_n48 < along && along < k_f_48) {
    along = k_f_0;
  }
  speed = k_f_0_002;
  if (g_player_inputs[1].insult != 0) {
    speed = k_f_1_0en4;
  }
  along = along * speed;
  step.x = along * *(float *)(frame + 0x30);
  step.y = *(float *)(frame + 0x34) * along;
  step.z = *(float *)(frame + 0x38) * along;
  RwMatrixTranslate(matrix, &step, rwCOMBINEPOSTCONCAT);
  along = -(float)(int)g_input_8c7e5b;
  if (k_f_n48 < along && along < k_f_48) {
    along = k_f_0;
  }
  speed = k_f_0_002;
  if (g_player_inputs[1].insult != 0) {
    speed = k_f_1_0en4;
  }
  along = along * speed;
  step.x = along * *(float *)(frame + 0x10);
  step.y = *(float *)(frame + 0x14) * along;
  step.z = *(float *)(frame + 0x18) * along;
  RwMatrixTranslate(matrix, &step, rwCOMBINEPOSTCONCAT);
}

