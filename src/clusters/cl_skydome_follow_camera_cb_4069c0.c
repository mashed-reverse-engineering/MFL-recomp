// cluster-entries: skydome_follow_camera_cb_4069c0
/* Installed cluster skydome_follow_camera_cb_4069c0 (tools/cluster_install.py from output/cluster_pilot_kd/skydome_follow_camera_cb_4069c0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0089de04*/ extern undefined4 g_app_89de04;
/* skydome_follow_camera_cb_4069c0 */

/* function pointer pushed/stored by skydome_follow_camera (PUSH 0x4069c0) */

void skydome_follow_camera_cb_4069c0(int dome_offset, undefined4 *camera_xy)

{
  int dome_entry;
  
  dome_entry = g_app_89de04 + dome_offset;
  if ((2 < *(int *)(g_app_89de04 + dome_offset)) && (*(int *)(g_app_89de04 + dome_offset) < 5)) {
    *(undefined4 *)(dome_entry + 0x20) = *camera_xy;
    *(undefined4 *)(dome_entry + 0x24) = camera_xy[1];
  }
  return;
}

