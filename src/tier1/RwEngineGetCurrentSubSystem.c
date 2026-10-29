/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*004d2e20*/ extern int __cdecl rwDeviceSystemRequest(void *engine, int op, void *a, void *b, int c);
/* Index of the current device sub-system, or -1. */
int RwEngineGetCurrentSubSystem(void)
{
  int n;
  if (rwDeviceSystemRequest(RwEngineInstance + 0x10, 0xf, &n, 0, 0)) return n;
  return -1;
}
