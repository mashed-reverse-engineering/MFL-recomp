/* compile: /G5 */
#include "mfl_tier2.h"
extern void (__cdecl *g_rt_7da8a8)(int);
typedef unsigned (__cdecl *FnU3)(void *, void *, unsigned);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* x */
unsigned RwFwrite_00560ee0(void *buf, unsigned size, unsigned n, void *file)
{
  char *fs = FLD(file, char *, 0x38) + 0x28;
  return ((FnU3)FLD(fs, void *, 0xc))(file, buf, size * n) / size;
}
