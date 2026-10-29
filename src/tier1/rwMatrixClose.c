/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007d2150*/ extern int g_rw_7d2150;
/*007d2154*/ extern int g_data_7d2154;
/*004e92b0*/ extern void __cdecl RwFreeListDestroy(void *fl);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Module close: destroys the matrix free list and decrements the open count; returns its argument. */
void *rwMatrixClose(void *p)
{
  void *fl = FLD(RwEngineInstance, void *, g_rw_7d2150);
  if (fl) { RwFreeListDestroy(fl); FLD(RwEngineInstance, int, g_rw_7d2150) = 0; }
  g_data_7d2154--;
  return p;
}
