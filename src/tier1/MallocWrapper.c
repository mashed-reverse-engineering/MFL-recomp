/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/* Stream allocation callback: engine allocation of the size stored in the context. */
void MallocWrapper(unsigned *ctx, unsigned hint)
{
  RwMalloc(*ctx, hint);
}
