/* compile: /G5 */
#include "mfl_tier2.h"
extern void (__cdecl *g_rt_7da8a8)(int);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* x */
void png_do_swap(unsigned char *ri, unsigned char *row)
{
  if (ri[9] == 16) { unsigned char *rp = row; unsigned i; unsigned n = ri[10] * FLD(ri, unsigned, 0); for (i = 0; i < n; i++) { unsigned char t = rp[0]; rp[0] = rp[1]; rp[1] = t; rp += 2; } }
}
