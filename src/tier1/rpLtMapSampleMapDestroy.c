/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/* Drops a reference of a light map sample map; frees it at zero; returns TRUE. */
int rpLtMapSampleMapDestroy(int *m)
{
  if (--*m == 0) RwFree(m);
  return 1;
}
