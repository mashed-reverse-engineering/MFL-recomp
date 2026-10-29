/* compile: /G5 */
#include "mfl_tier2.h"
/*007da898*/ extern int g_rt_7da898;
/*007da89c*/ extern int g_rt_7da89c;
extern void (__cdecl *g_rt_7da8a8)(int);
/* x */
int rtFSManagerIsNewFSRegistrable(void)
{
  if (g_rt_7da898 > 0 && g_rt_7da89c + 1 > g_rt_7da898) { if (g_rt_7da8a8) g_rt_7da8a8(5); return 0; }
  return 1;
}
