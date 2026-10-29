/* compile: /G5 */
#include "mfl_tier2.h"
extern void (__cdecl *g_rt_7da8a8)(int);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* x */
int png_get_PLTE(void *png, void *info, void **pal, int *num)
{
  if (png && info && (FLD(info, unsigned char, 8) & 8) && pal) { *pal = FLD(info, void *, 0x10); *num = FLD(info, unsigned short, 0x14); return 8; }
  return 0;
}
