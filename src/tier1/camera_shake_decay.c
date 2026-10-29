#include "mfl_tier2.h"
/*005e64a4*/ extern float k_f_0;
/*007ef7ac*/ extern float g_camera_shake;
/* camera_shake_decay */
/* "(dt): g_camera_7ef7ac -= dt, clamped to 0" */
/* Decays the global camera shake g_camera_shake by dt each call, clamped to 0. No return value.
   dt: amount subtracted from g_camera_shake. */
void camera_shake_decay(float dt)
{
  /* Subtract only while shake is above k_f_0; once it drops below k_f_0, snap it to 0.0. */
  if ((g_camera_shake > k_f_0) &&
     (g_camera_shake = g_camera_shake - dt, g_camera_shake < k_f_0)) {
    g_camera_shake = 0.0;
  }
  return;
}
