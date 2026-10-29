/* compile: /G5 */
#include "mfl_tier2.h"
extern void (__cdecl *g_rt_7da8a8)(int);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* x */
void png_do_invert(unsigned char *ri, unsigned char *row)
{
  if (ri[9] == 1 && ri[8] == 0) { unsigned char *rp = row; unsigned i; unsigned n = FLD(ri, unsigned, 4); for (i = 0; i < n; i++, rp++) *rp = ~*rp; }
}
