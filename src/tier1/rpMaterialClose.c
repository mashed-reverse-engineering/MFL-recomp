/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007d5390*/ extern int g_rp_7d5390;
/*007d5394*/ extern int g_data_7d5394;
/*004e92b0*/ extern void __cdecl RwFreeListDestroy(void *fl);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Module close: destroys the material free list and decrements the open count; returns its argument. */
void *rpMaterialClose(void *p)
{
  void *fl = FLD(RwEngineInstance, void *, g_rp_7d5390);
  if (fl) { RwFreeListDestroy(fl); FLD(RwEngineInstance, int, g_rp_7d5390) = 0; }
  g_data_7d5394--;
  return p;
}
