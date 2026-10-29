/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/* PNG memory callback: engine allocation of the requested size. */
void *PNGMalloc(void *ctx, unsigned size)
{
  return RwMalloc(size, 0x3010d);
}
