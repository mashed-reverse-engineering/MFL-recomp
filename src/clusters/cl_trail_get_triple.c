// cluster-entries: trail_get_triple
/* Installed cluster trail_get_triple (tools/cluster_install.py from output/cluster_pilot_kd/trail_get_triple.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00652fb0*/ extern undefined g_trail_652fb0;
/*00652fb4*/ extern undefined g_trail_652fb4;
/*00652fb8*/ extern undefined g_trail_652fb8;
/* trail_get_triple */

/* "(a, b, out xyz): three floats of the record at 0x652FB0 (stride 0xC)" */

void trail_get_triple(int a, int b, undefined4 *out_xyz)

{
  int offset;
  
  offset = (b + a * 2) * 0xc;
  *out_xyz = *(undefined4 *)((char *)&g_trail_652fb0 + offset);
  out_xyz[1] = *(undefined4 *)((char *)&g_trail_652fb4 + offset);
  out_xyz[2] = *(undefined4 *)((char *)&g_trail_652fb8 + offset);
  return;
}

