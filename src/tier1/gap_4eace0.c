/* compile: /G5 */
#include "mfl_tier2.h"
/*007d51c4*/ extern int g_data_7d51c4;
/* Module close hook: decrements the open count, returns its argument. */
void *gap_4eace0(void *p)
{
  g_data_7d51c4--;
  return p;
}
