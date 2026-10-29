/* compile: /G5 */
#include "mfl_tier2.h"
/*007da6e4*/ extern int g_data_7da6e4;
/* Module close hook: decrements the open count, returns its argument. */
void *CollisionClose(void *p)
{
  g_data_7da6e4--;
  return p;
}
