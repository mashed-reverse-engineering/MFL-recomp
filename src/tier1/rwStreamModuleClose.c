/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007d26f4*/ extern int g_rw_7d26f4;
/*007d26f8*/ extern int g_data_7d26f8;
/*004e92b0*/ extern void __cdecl RwFreeListDestroy(void *fl);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Module close: destroys the stream free list and decrements the open count; returns its argument. */
void *rwStreamModuleClose(void *p)
{
  void *fl = FLD(RwEngineInstance, void *, g_rw_7d26f4);
  if (fl) RwFreeListDestroy(fl);
  g_data_7d26f8--;
  return p;
}
