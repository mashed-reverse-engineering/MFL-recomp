/* compile: /G5 */
#include "mfl_tier2.h"
/*004f56f0*/ extern void *__cdecl RpWorldSectorGetWorld(void *sector);
/*00501140*/ extern void *__cdecl rpMeshRead(void *s, void *w, void *e);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream read of a world sector's mesh header; returns the stream or NULL. */
void *readSectorMesh(void *s, void *p, void *sec)
{
  char *w = RpWorldSectorGetWorld(sec);
  void *m = rpMeshRead(s, w, w + 0x10);
  FLD(sec, void *, 0x78) = m;
  return m ? s : 0;
}
