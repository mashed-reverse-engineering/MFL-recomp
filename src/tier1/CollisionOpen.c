/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007da6e8*/ extern int g_rp_7da6e8;
/*007da6e4*/ extern int g_data_7da6e4;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Module open hook: increments the open count, clears the engine collision global; returns its argument. */
void *CollisionOpen(void *p)
{
  g_data_7da6e4++;
  FLD(RwEngineInstance, int, g_rp_7da6e8) = 0;
  return p;
}
