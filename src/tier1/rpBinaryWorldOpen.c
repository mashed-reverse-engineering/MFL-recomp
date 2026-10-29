/* compile: /G5 */
#include "mfl_tier2.h"
/*007d53b4*/ extern int g_data_7d53b4;
/* Module open hook: increments the open count, returns its argument. */
void *rpBinaryWorldOpen(void *p)
{
  g_data_7d53b4++;
  return p;
}
