/* compile: /G5 */
#include "mfl_tier2.h"
/*00500ff0*/ extern int __cdecl rpMeshWrite(void *mesh, void *s, void *p, void *q);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream write of a geometry's mesh header. */
int writeGeometryMesh(void *s, void *p, void *g)
{
  return rpMeshWrite(FLD(g, void *, 0x54), g, s, (char *)g + 0x20);
}
