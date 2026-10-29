/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef void *(__cdecl *RwCallocFn)(unsigned, unsigned, unsigned);
/* Allocates a zeroed bit grid of a x b bits (32 bit words plus a 12 byte header) and stores width, height and word count. (Rwp library, proposed name rwp_trilist_00573f10) */
void *FUN_00573f10(unsigned a, unsigned b)
{
  unsigned n = (a * b + 0x1f) >> 5;
  unsigned *p = (*(RwCallocFn *)(RwEngineInstance + 0x114))(n + 3, 4, 0x30900);
  p[0] = a;
  p[2] = n;
  p[1] = b;
  return p;
}
