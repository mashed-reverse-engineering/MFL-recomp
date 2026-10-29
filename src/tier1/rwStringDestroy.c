/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/* Frees a string through the engine free hook; returns TRUE. */
int rwStringDestroy(void *s)
{
  RwFree(s);
  return 1;
}
