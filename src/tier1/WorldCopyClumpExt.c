/* compile: /G5 */
#include "mfl_tier2.h"
/*007d52a4*/ extern int g_rp_7d52a4;
/*004f4570*/ extern void *__cdecl RpWorldAddClump(void *w, void *c);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Clump plugin copy: adds the source's world to the destination clump; returns the destination. */
void *WorldCopyClumpExt(void *dst, void *src)
{
  void *p = FLD(src, void *, g_rp_7d52a4);
  if (p) RpWorldAddClump(p, dst);
  return dst;
}
