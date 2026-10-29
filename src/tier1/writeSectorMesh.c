/* compile: /G5 */
#include "mfl_tier2.h"
/*004f56f0*/ extern void *__cdecl RpWorldSectorGetWorld(void *sector);
/*00500ff0*/ extern int __cdecl rpMeshWrite(void *mesh, void *w, void *s, void *e);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream write of a world sector's mesh header. */
int writeSectorMesh(void *s, void *p, void *sec)
{
  char *w = RpWorldSectorGetWorld(sec);
  return rpMeshWrite(FLD(sec, void *, 0x78), w, s, w + 0x10);
}
