/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007d21d8*/ extern int g_rw_7d21d8;
/*007d21dc*/ extern int g_data_7d21dc;
/*004e92b0*/ extern void __cdecl RwFreeListDestroy(void *fl);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Module close: destroys the raster free list and decrements the open count; returns its argument. */
void *rwRasterClose(void *p)
{
  void *fl = FLD(RwEngineInstance, void *, g_rw_7d21d8 + 0x60);
  if (fl) { RwFreeListDestroy(fl); FLD(RwEngineInstance, int, g_rw_7d21d8 + 0x60) = 0; }
  g_data_7d21dc--;
  return p;
}
