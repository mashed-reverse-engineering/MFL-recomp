/* compile: /G5 */
#include "mfl_tier2.h"
/*007d54d4*/ extern int g_data_7d54d4;
/* Module close hook: decrements the open count, returns its argument. */
void *rpD3D9UsageFlagClose(void *p)
{
  g_data_7d54d4--;
  return p;
}
