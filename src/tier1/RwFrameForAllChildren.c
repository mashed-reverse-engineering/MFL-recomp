/* compile: /G5 */
#include "mfl_tier2.h"
typedef int (__cdecl *ForAllFn)(void *, void *);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Calls a callback for every child frame until it returns NULL; returns the frame. */
void *RwFrameForAllChildren(void *f, ForAllFn cb, void *data)
{
  void *c = FLD(f, void *, 0x98);
  while (c) { void *next = FLD(c, void *, 0x9c); if (!cb(c, data)) break; c = next; }
  return f;
}
