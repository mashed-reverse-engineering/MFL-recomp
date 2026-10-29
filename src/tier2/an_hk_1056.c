#include "mfl_tier2.h"
/*004757a0*/ extern int get_viewport_camera_matrix();
/*004a1680*/ extern int player_rumble();
/*004b2b48*/ extern int _ftol();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005e64a4*/ extern float k_f_0;
/*007ef7ac*/ extern float g_camera_shake;
/* course_effect_spawn_at_helper_45c8f0 */

/* callers course_effect_spawn_at */

void course_effect_spawn_at_helper_45c8f0(float *pos,float range)

{
  double shake;
  double falloff;
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
  shake = falloff * k_f_0_5;
  g_camera_shake = shake;
  if (k_f_0 < shake) {
    i = 0;
    /* the rumble strength reuses the shake value (x87 temporary, unrounded) */
    cam_matrix_or_alpha = (int)(shake * (-127.0f));
    do {
      player_rumble(i,2,0xf,0x80 - cam_matrix_or_alpha);
      i = i + 1;
    } while (i < 4);
    return;
  }
  return;
}


