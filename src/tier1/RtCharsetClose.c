/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007da9f8*/ extern int g_rt_7da9f8;
/*007da9fc*/ extern int g_rt_7da9fc;
/*007da9ec*/ extern int g_rt_7da9ec;
/* Module close: frees the two charset buffers and clears the state. */
void RtCharsetClose(void)
{
  RwFree((void *)g_rt_7da9f8);
  g_rt_7da9f8 = 0;
  RwFree((void *)g_rt_7da9fc);
  g_rt_7da9fc = 0;
  g_rt_7da9ec = 0;
}
