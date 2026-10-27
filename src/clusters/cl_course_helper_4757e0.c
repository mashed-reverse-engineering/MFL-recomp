// cluster-entries: course_helper_4757e0
/* Installed cluster course_helper_4757e0 (tools/cluster_install.py from output/cluster_pilot_kb/course_helper_4757e0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00475760*/ extern int aux_camera_get_position();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*006c294c*/ extern float g_data_6c294c;
/*006c2950*/ extern float g_data_6c2950;
/*006c2954*/ extern float g_data_6c2954;
/* course_helper_4757e0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* callers course_angels_cb1,course_keys_cb1,course_neustein_cb1,course_nt1_s_cb1 */

float10 course_helper_4757e0(undefined4 viewport)

{
  float *cam;
  int cam_record;
  undefined4 *cam_now;
  RwReal dist;
  RwV3d delta;
  
  cam = (float *)aux_camera_get_position(viewport);
  delta.x = *cam - g_data_6c294c;
  cam_record = aux_camera_get_position(viewport);
  delta.y = *(float *)(cam_record + 4) - g_data_6c2950;
  cam_record = aux_camera_get_position(viewport);
  delta.z = *(float *)(cam_record + 8) - g_data_6c2954;
  cam_now = (undefined4 *)aux_camera_get_position(viewport);
  *(undefined4 *)&g_data_6c294c = cam_now[0]; /* the previous position is kept as a raw copy of the three words */
  *(undefined4 *)&g_data_6c2950 = cam_now[1];
  *(undefined4 *)&g_data_6c2954 = cam_now[2];
  dist = RwV3dLength(&delta);
  return (float10)dist;
}

