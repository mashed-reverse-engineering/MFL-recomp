#include "mfl_tier2.h"
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*0064d950*/ extern undefined4 g_crown_64d950;


/* crown_destroy */

/* "RpClumpDestroy(0x64D950)" */

void crown_destroy(void)
{
  RpClumpDestroy(g_crown_64d950);
  return;
}
