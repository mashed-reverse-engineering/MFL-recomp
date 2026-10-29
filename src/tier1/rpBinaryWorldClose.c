/* compile: /G5 */
#include "mfl_tier2.h"
/*007d53b4*/ extern int g_data_7d53b4;
/* Module close hook: decrements the open count, returns its argument. */
void *rpBinaryWorldClose(void *p)
{
  g_data_7d53b4--;
  return p;
}
