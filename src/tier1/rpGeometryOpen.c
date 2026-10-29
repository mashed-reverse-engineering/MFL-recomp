/* compile: /G5 */
#include "mfl_tier2.h"
/*007d539c*/ extern int g_data_7d539c;
/*007d5398*/ extern int g_data_7d5398;
/* Module open hook: stores the plugin offset argument, increments the open count, returns its argument. */
void *rpGeometryOpen(void *p, int v)
{
  g_data_7d5398 = v;
  g_data_7d539c++;
  return p;
}
