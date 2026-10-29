/* compile: /G5 */
#include "mfl_tier2.h"
/*00501140*/ extern void *__cdecl rpMeshRead(void *s, void *p, void *q);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream read of a geometry's mesh header; returns the stream or NULL. */
void *readGeometryMesh(void *s, void *p, void *g)
{
  void *m = rpMeshRead(s, g, (char *)g + 0x20);
  FLD(g, void *, 0x54) = m;
  return m ? s : 0;
}
