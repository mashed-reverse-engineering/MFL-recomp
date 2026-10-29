/* compile: /G5 */
#include "mfl_tier2.h"
/*00947b1c*/ extern int g_data_947b1c;
/* Module open hook: increments the open count, returns its argument. */
void *UserDataOpen(void *p)
{
  g_data_947b1c++;
  return p;
}
