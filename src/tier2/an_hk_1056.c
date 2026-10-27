#include "mfl_tier2.h"
/*004757a0*/ extern int get_viewport_camera_matrix();
/*004a1680*/ extern int player_rumble();
/*004b2b48*/ extern int _ftol();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005ddc28*/ extern float k_f_0;
/*007ef7ac*/ extern float g_camera_shake;
/* course_effect_spawn_at_helper_45c8f0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* callers course_effect_spawn_at */

void course_effect_spawn_at_helper_45c8f0(float *pos,float range)

{
  float fv1;
  float falloff;
  int cam_matrix_or_alpha;
  int i;
  RwReal cam_dist;
  RwV3d to_camera;
  
  cam_matrix_or_alpha = get_viewport_camera_matrix(0);
  to_camera.x = *(float *)(cam_matrix_or_alpha + 0x30) - *pos;
  cam_matrix_or_alpha = get_viewport_camera_matrix(0);
  to_camera.y = *(float *)(cam_matrix_or_alpha + 0x34) - pos[1];
  cam_matrix_or_alpha = get_viewport_camera_matrix(0);
  to_camera.z = *(float *)(cam_matrix_or_alpha + 0x38) - pos[2];
  cam_dist = RwV3dLength(&to_camera);
  falloff = float_1_0 - (float_1_0 / range) * cam_dist;
  if (falloff < k_f_0) {
    falloff = k_f_0;
  }
  g_camera_shake = falloff * k_f_0_5;
  if (k_f_0 < g_camera_shake) {
    i = 0;
    fv1 = (1.0f - (cam_dist * (1.0f / range)));
    cam_matrix_or_alpha = ((int)(((!(fv1 < 0.0f) ? fv1 : 0.0f) * 0.5f) * (-127.0f)));
    do {
      player_rumble(i,2,0xf,0x80 - cam_matrix_or_alpha);
      i = i + 1;
    } while (i < 4);
    return;
  }
  return;
}


