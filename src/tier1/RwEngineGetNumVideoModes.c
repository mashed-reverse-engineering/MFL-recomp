/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*004d2e20*/ extern int __cdecl rwDeviceSystemRequest(void *engine, int op, void *a, void *b, int c);
/* Number of video modes of the current sub-system, or -1. */
int RwEngineGetNumVideoModes(void)
{
  int n;
  if (rwDeviceSystemRequest(RwEngineInstance + 0x10, 5, &n, 0, 0)) return n;
  return -1;
}
