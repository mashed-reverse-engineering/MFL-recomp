/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007d5194*/ extern int g_data_7d5194;
/*007d5198*/ extern int g_data_7d5198;
/*004e92b0*/ extern void __cdecl RwFreeListDestroy(void *fl);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Module close: destroys the chunk group free list and decrements the open count; returns its argument. */
void *rwChunkGroupClose(void *p)
{
  void *fl = FLD(RwEngineInstance, void *, g_data_7d5194);
  if (fl) RwFreeListDestroy(fl);
  g_data_7d5198--;
  return p;
}
