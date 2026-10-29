/* compile: /G5 */
#include "mfl_tier2.h"
/*0054dcb0*/ extern int __cdecl rpCollTreeStreamGetSize(void *t);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream size of the collision tree stored in a plugin slot: tree size plus 16, or 0 without a tree. */
int CollisionDataStreamGetSize(void *o, int off)
{
  void *p = FLD(o, void *, off);
  if (p) return rpCollTreeStreamGetSize(p) + 0x10;
  return 0;
}
