#include "mfl_tier2.h"
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006592d4*/ extern undefined4 g_hudpanel_6592d4;


/* hudpanel_clump_destroy */

/* "RpClumpDestroy(g_hudpanel_6592d4)" */

void hudpanel_clump_destroy(void)
{
  RpClumpDestroy(g_hudpanel_6592d4);
  return;
}
