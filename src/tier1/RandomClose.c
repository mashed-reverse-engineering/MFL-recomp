/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007da6b8*/ extern int g_rp_7da6b8;
/* Module close: frees the random generator state and clears the slot; returns its argument. */
void *RandomClose(void *x)
{
  void **p = (void **)(RwEngineInstance + g_rp_7da6b8);
  if (*p) { RwFree(*p); *(int *)(RwEngineInstance + g_rp_7da6b8) = 0; }
  return x;
}
