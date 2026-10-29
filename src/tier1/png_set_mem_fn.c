/* compile: /G5 */
#include "mfl_tier2.h"
extern void (__cdecl *g_rt_7da8a8)(int);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* x */
void png_set_mem_fn(void *png, void *mem, void *m, void *f)
{
  FLD(png, void *, 0x1e8) = mem;
  FLD(png, void *, 0x1ec) = m;
  FLD(png, void *, 0x1f0) = f;
}
