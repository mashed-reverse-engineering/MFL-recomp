/* compile: /G5 */
#include "mfl_tier2.h"
/*00947b24*/ extern int g_data_947b24;
/* Module close hook: decrements the open count, returns its argument. */
void *AnimClose(void *p)
{
  g_data_947b24--;
  return p;
}
