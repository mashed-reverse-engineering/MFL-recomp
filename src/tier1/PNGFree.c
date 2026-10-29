/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/* PNG memory callback: engine free. */
void PNGFree(void *ctx, void *p)
{
  RwFree(p);
}
