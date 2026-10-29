/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*005ea284*/ extern char g_rw_5ea284[];
/* Stream size of a string: length plus terminator rounded up to a multiple of 4. */
int rwStringStreamGetSize(char *s)
{
  if (!s) s = g_rw_5ea284;
  return (rwstrlen(s) + 4) & ~3;
}
