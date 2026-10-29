/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Frees the file object table of a file system. */
void TkWinFSClose(void *fs)
{
  RwFree(FLD(fs, void *, 0x5c));
}
