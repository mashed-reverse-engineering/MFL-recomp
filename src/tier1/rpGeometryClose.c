/* compile: /G5 */
#include "mfl_tier2.h"
/*007d539c*/ extern int g_data_7d539c;
/* Module close hook: decrements the open count, returns its argument. */
void *rpGeometryClose(void *p)
{
  g_data_7d539c--;
  return p;
}
