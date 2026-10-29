#include "mfl_tier2.h"
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*00658b40*/ extern undefined4 g_btb_658b40;
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*00658b40*/ extern undefined4 g_btb_658b40;
/* btb_panel_destroy */

/* "RpClumpDestroy(0x658B40) (BTBPanel.dff, inferred)" */

void btb_panel_destroy(void)
{
  RpClumpDestroy(g_btb_658b40);
  return;
}
