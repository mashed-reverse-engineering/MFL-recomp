#include "mfl_tier2.h"
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*00659b00*/ extern undefined4 g_stopclock_659b00;


/* stopclock_destroy */

/* "RpClumpDestroy(0x659B00)" */

void stopclock_destroy(void)
{
  RpClumpDestroy(g_stopclock_659b00);
  return;
}
