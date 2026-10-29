/* compile: /G5 */
#include "mfl_tier2.h"
extern void (__cdecl *g_rt_7da8a8)(int);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* x */
unsigned png_get_valid(void *png, void *info, unsigned flag)
{
  if (png && info) return FLD(info, unsigned, 8) & flag;
  return 0;
}
