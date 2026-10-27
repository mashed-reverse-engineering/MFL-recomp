// cluster-entries: scene_object_select_set
/* Installed cluster scene_object_select_set (tools/cluster_install.py from output/cluster_pilot_kd/scene_object_select_set.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00651240*/ extern undefined4 g_scene_selected_object;
/*00651244*/ extern float g_scene_select_timer;
/*00651248*/ extern undefined4 g_scene_select_param;
/* scene_object_select_set */

/* "stores the selected object and its parameter or clears them when null" */

void scene_object_select_set(int param_1, undefined4 param_2, char *unused_3, char *unused_4, char *unused_5)

{
  if (param_1 != 0) {
    g_scene_select_timer = 0;
    g_scene_selected_object = param_1;
    g_scene_select_param = param_2;
    return;
  }
  g_scene_selected_object = 0;
  g_scene_select_param = 0;
  return;
}

