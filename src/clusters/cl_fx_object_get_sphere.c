// cluster-entries: fx_object_get_sphere
/* Installed cluster fx_object_get_sphere (tools/cluster_install.py from output/cluster_pilot_kb/fx_object_get_sphere.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00651260*/ extern undefined4 g_fx_651260;
/*00651298*/ extern undefined4 g_fx_651298;
/* fx_object_get_sphere */

/* "(index, out xyzr): position of the object frame and radius 2.0; returns whether inactive" */

bool fx_object_get_sphere(int index, undefined4 *out_xyzr)

{
  char *frame_ptr;
  undefined4 pos_z;
  
  frame_ptr = *(char **)((&g_fx_651260)[index * 0x3b] + 4);
  *out_xyzr = *(undefined4 *)(frame_ptr + 0x40);
  out_xyzr[1] = *(undefined4 *)(frame_ptr + 0x44);
  pos_z = *(undefined4 *)(frame_ptr + 0x48);
  out_xyzr[3] = 0x40000000;
  out_xyzr[2] = pos_z;
  return (&g_fx_651298)[index * 0x3b] == 0;
}

