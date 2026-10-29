/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007d518c*/ extern int g_rw_7d518c;
/*007d5190*/ extern int g_data_7d5190;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Module open: increments the open count, remembers the plugin offset and initialises the error state; returns its argument. */
void *rwErrorOpen(void *p, int v)
{
  int c = g_data_7d5190 + 1;
  g_rw_7d518c = v;
  g_data_7d5190 = c;
  FLD(RwEngineInstance, int, v) = 0;
  FLD(RwEngineInstance, int, g_rw_7d518c + 4) = 0x80000000;
  return p;
}
