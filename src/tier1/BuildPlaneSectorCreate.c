/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef void *(__cdecl *RwAllocFn)(unsigned, unsigned);
extern void (__cdecl *g_rx_7d53c4)(void);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Allocates a 0x14 byte plane sector record and stores three values; NULL on failure. */
void *BuildPlaneSectorCreate(int a, int b, int c)
{
  char *p = (char *)(*(RwAllocFn *)(RwEngineInstance + 0x108))(0x14, 0);
  if (!p) return p;
  FLD(p, int, 0) = a;
  FLD(p, int, 0xc) = b;
  FLD(p, int, 0x10) = c;
  return p;
}
