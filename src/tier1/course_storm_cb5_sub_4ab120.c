#include "mfl_tier2.h"
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*00771ab0*/ extern struct RpAtomic *g_data_771ab0;
/* course_storm_cb5_sub_4ab120 */

/* single caller course_storm_cb5 */

void course_storm_cb5_sub_4ab120(void)

{
  RwFrameDestroy(RpAtomic_object_object_parent(g_data_771ab0));
  RpAtomicDestroy(g_data_771ab0);
  return;
}
