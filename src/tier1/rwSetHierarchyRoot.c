/* compile: /G5 */
#include "mfl_tier2.h"
extern void (__cdecl *g_rx_7d53c4)(void);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the hierarchy root of a frame and, recursively, of all its children. */
void rwSetHierarchyRoot(void *f, void *root)
{
  void *c;
  FLD(f, void *, 0xa0) = root;
  for (c = FLD(f, void *, 0x98); c; c = FLD(c, void *, 0x9c)) rwSetHierarchyRoot(c, root);
}
