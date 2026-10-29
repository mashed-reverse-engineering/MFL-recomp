/* compile: /G5 */
#include "mfl_tier2.h"
/*00948094*/ extern int g_rt_948094;
/*00948098*/ extern int g_rt_948098;
/*004e92b0*/ extern void __cdecl RwFreeListDestroy(void *fl);
/* Module close: destroys the two triangulation free lists and clears the pointers. */
void rt2dTriangulateClose(void)
{
  RwFreeListDestroy((void *)g_rt_948094);
  RwFreeListDestroy((void *)g_rt_948098);
  g_rt_948094 = 0;
  g_rt_948098 = 0;
}
