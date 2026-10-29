/* compile: /G5 */
#include "mfl_tier2.h"
extern void __cdecl rwFrameSyncHierarchyLTM(void *root);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Local transformation matrix of a frame; resynchronises the hierarchy first when it is dirty. */
void *RwFrameGetLTM(void *f)
{
  void *root = FLD(f, void *, 0xa0);
  if (FLD(root, unsigned char, 3) & 1) rwFrameSyncHierarchyLTM(root);
  return (char *)f + 0x50;
}
