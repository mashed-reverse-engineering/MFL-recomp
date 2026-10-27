// cluster-entries: course_helper_45bad0
/* Installed cluster course_helper_45bad0 (tools/cluster_install.py from output/cluster_pilot_kb/course_helper_45bad0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00408ea0*/ extern int __cdecl player_record_get_8d8ca0(int index);
/*0045b7a0*/ extern void course_helper_45bad0_sub_45b7a0(void *effect);
/*0045b8c0*/ extern void course_effect_spawn_at(void *effect);
/*0045c9c0*/ extern void camera_shake_decay(float amount);
/*00481ed0*/ extern int __cdecl table_get_6c3f38(int index);
/*00481ee0*/ extern void __cdecl player_marker_set_enabled(int marker, int enabled);
/*005dda1c*/ extern float k_f_3_3333333en4;
/*0060d960*/ extern int g_data_60d960;
/*006a26c0*/ extern undefined4 g_course_6a26c0;
/*006a2720*/ extern undefined4 g_course_6a2720;
/*006a2784*/ extern int g_course_6a2784;
/*006a278c*/ extern int g_data_6a278c;
/*006a2790*/ extern int g_data_6a2790;
/* course_helper_45bad0 */

/* Per-frame update of the sands course effect trio: tracks the player record value, advances the
   effect stage (g_course_6a2784, 1..3) while the record sits in 0x37..0x3c, decays the camera shake
   and, once the stage's table entry is set, enables the player markers and spawns the stage effect. */
void course_helper_45bad0(float frame_time)

{
  g_data_6a278c = g_data_6a2790;
  g_data_6a2790 = player_record_get_8d8ca0(0);
  if ((g_data_6a278c == 0) && (g_data_6a2790 == 1)) {
    g_data_60d960 = g_data_6a2790;
  }
  course_helper_45bad0_sub_45b7a0(&g_course_6a26c0);
  course_helper_45bad0_sub_45b7a0(&g_course_6a2720);
  if (g_data_60d960 != 0) {
    if (g_course_6a2784 == 3) goto decay;
    if ((0x36 < g_data_6a2790) && (g_data_6a2790 < 0x3d)) {
      g_course_6a2784 = g_course_6a2784 + 1;
    }
  }
  if (3 < g_course_6a2784) {
    g_course_6a2784 = 3;
  }
decay:
  camera_shake_decay(frame_time * k_f_3_3333333en4);
  if (g_course_6a2784 == 1) {
    if (table_get_6c3f38(0) != 0) {
      player_marker_set_enabled(0,0);
      player_marker_set_enabled(1,1);
      course_effect_spawn_at(&g_course_6a26c0);
      g_data_60d960 = 0;
    }
  }
  else if (g_course_6a2784 == 2) {
    if (table_get_6c3f38(1) != 0) {
      player_marker_set_enabled(1,0);
      player_marker_set_enabled(2,1);
      course_effect_spawn_at(&g_course_6a2720);
      g_data_60d960 = 0;
    }
  }
  else if (g_course_6a2784 == 3) {
    if (table_get_6c3f38(2) != 0) {
      player_marker_set_enabled(2,0);
      player_marker_set_enabled(3,1);
      g_data_60d960 = 0;
    }
  }
}

