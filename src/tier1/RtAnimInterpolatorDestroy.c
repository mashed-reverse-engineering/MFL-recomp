/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/* Frees an interpolator through the engine free hook. */
void RtAnimInterpolatorDestroy(void *p)
{
  RwFree(p);
}
