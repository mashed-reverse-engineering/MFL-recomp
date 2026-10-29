/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*00948060*/ extern void *g_rt_948060;
/*00948064*/ extern void **g_rt_948064;
/*004e92b0*/ extern void __cdecl RwFreeListDestroy(void *fl);
/* Module close: destroys the brush free list and frees the brush buffer. */
void rt2dBrushClose(void)
{
  RwFreeListDestroy(g_rt_948060);
  g_rt_948060 = 0;
  RwFree(g_rt_948064);
  g_rt_948064 = 0;
}
