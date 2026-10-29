/* compile: /G5 */
#include "mfl_tier2.h"
/*007d53bc*/ extern int g_data_7d53bc;
/* Module open hook: increments the open count, returns its argument. */
void *rpSectorOpen(void *p)
{
  g_data_7d53bc++;
  return p;
}
