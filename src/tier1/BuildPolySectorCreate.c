/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef void *(__cdecl *RwAllocFn)(unsigned, unsigned);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Allocates an 8 byte polygon sector record (id -1 and two 16 bit values); NULL on failure. */
void *BuildPolySectorCreate(short a, short b)
{
  char *p = (char *)(*(RwAllocFn *)(RwEngineInstance + 0x108))(8, 0);
  if (!p) return p;
  FLD(p, int, 0) = -1;
  FLD(p, short, 4) = a;
  FLD(p, short, 6) = b;
  return p;
}
