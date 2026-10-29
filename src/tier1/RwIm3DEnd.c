/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*00947360*/ extern int g_rw_947360;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Ends immediate 3D rendering: clears the 15 dword state block; FALSE when it was not started. */
int RwIm3DEnd(void)
{
  char *p = RwEngineInstance + g_rw_947360;
  if (!FLD(p, int, 0x3c)) return 0;
  memset(p + 0x38, 0, 60);
  return 1;
}
