#include "mfl_tier2.h"
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*00659a9c*/ extern undefined4 g_load_659a9c;


/* team_endpoint_panel_destroy */

/* "RpClumpDestroy(0x659A9C)" */

void team_endpoint_panel_destroy(void)
{
  RpClumpDestroy(g_load_659a9c);
  return;
}
