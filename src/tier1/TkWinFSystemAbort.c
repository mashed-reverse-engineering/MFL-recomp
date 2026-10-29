/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Marks a file object as aborted; returns TRUE. */
int TkWinFSystemAbort(void *fs)
{
  FLD(fs, int, 0x20) = 1;
  return 1;
}
