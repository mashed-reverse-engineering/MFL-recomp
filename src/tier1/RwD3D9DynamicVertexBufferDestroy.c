/* compile: /G5 */
#include "mfl_tier2.h"
/*007d51f0*/ extern char *g_rw_7d51f0;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Finds the dynamic vertex buffer entry and clears its buffer / size. */
void RwD3D9DynamicVertexBufferDestroy(void *vb)
{
  char *e = g_rw_7d51f0;
  while (e) { if (FLD(e, void *, 8) == vb) { FLD(e, int, 4) = 0; FLD(e, int, 0xc) = 0; return; } e = FLD(e, char *, 0x10); }
}
