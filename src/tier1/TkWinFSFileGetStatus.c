/* compile: /G5 */
#include "mfl_tier2.h"
/*005615f0*/ extern void __cdecl TkWinFSystemFSync(void *f, int a);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Syncs the file object when it is not in state 1 and returns its status. */
int TkWinFSFileGetStatus(void *f)
{
  if (FLD(f, int, 0x20) != 1) TkWinFSystemFSync(f, 0);
  return FLD(f, int, 0x20);
}
