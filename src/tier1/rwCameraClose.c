/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007d1ff0*/ extern int g_rw_7d1ff0;
/*007d1ff4*/ extern int g_data_7d1ff4;
/*004e92b0*/ extern void __cdecl RwFreeListDestroy(void *fl);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Module close: destroys the camera free list and decrements the open count; returns its argument. */
void *rwCameraClose(void *p)
{
  void *fl = FLD(RwEngineInstance, void *, g_rw_7d1ff0);
  if (fl) { RwFreeListDestroy(fl); FLD(RwEngineInstance, int, g_rw_7d1ff0) = 0; }
  g_data_7d1ff4--;
  return p;
}
