// cluster-entries: gap_4c47c0
/* Installed cluster gap_4c47c0 (tools/cluster_install.py from output/cluster_pilot_md/gap_4c47c0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00544440*/ extern int __cdecl rwframe_plugin_object_get(RwFrame *frame);
/*004d1420*/ extern struct RwFrame * RwFrameForAllChildren(struct RwFrame *p0, RwFrameCallBack p1, void *p2);
/* gap_4c47c0 */

/* "RwFrameForAllChildren callback: depth first search below a frame for the first frame that carries a plugin object; stores the object
   behind data and stops the walk (returns NULL), otherwise recurses into the children" */
RwFrame *gap_4c47c0(RwFrame *frame, void *data)
{
  int object;

  object = rwframe_plugin_object_get(frame);
  if (object == 0) {
    RwFrameForAllChildren(frame, (RwFrameCallBack)gap_4c47c0, data);
    return frame;
  }
  *(int *)data = object;
  return 0;
}

