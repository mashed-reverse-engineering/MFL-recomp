/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Entry i (0x60 bytes each) of the file system object table, or NULL when out of range. */
void *TkWinFSGetFileObject(void *fs, unsigned i)
{
  if (i < FLD(fs, unsigned, 4)) return FLD(fs, char *, 0x5c) + i * 0x60;
  return 0;
}
