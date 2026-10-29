/* compile: /G5 */
#include "mfl_tier2.h"
/*007d5190*/ extern int g_data_7d5190;
/* Module close hook: decrements the open count, returns its argument. */
void *rwErrorClose(void *p)
{
  g_data_7d5190--;
  return p;
}
